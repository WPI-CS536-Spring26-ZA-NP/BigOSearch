#include "bigoFinder.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <Eigen/Dense>
#include <cmath>
#include <limits>
#include <string>

namespace bigO_Finder
{

    std::tuple<bool, expectedOutput> *mailbox = nullptr;
    std::thread *threads = nullptr;

    std::tuple<bool, std::map<input, std::tuple<bool, output, expectedOutput>>>
    testingFunction(functionTester tester, EQFunc eqFunc, std::map<input, expectedOutput> map, size_t OutputTypeSize)
    {

        std::map<input, std::tuple<bool, output, expectedOutput>> outmap;
        bool res = true;

        for (auto &&i : map)
        {

            void *resp = new char[OutputTypeSize];
            output oput = tester(i.first, resp);
            bool valid = eqFunc(oput, i.second);
            outmap[i.first] = std::tuple(valid, oput, i.second);
            if (!valid)
            {
                res = false;
            }
        }
        return std::tuple(res, outmap);
    }

    void cleanUpTestResults(std::tuple<bool, std::map<input, std::tuple<bool, output, expectedOutput>>> result, outputCleanup g)
    {
        auto map = std::get<1>(result);
        for (auto &m : map)
        {
            g((std::get<1>(m.second)));
        }
    }

    struct timespec diff_timespec(const struct timespec *time1,
                                  const struct timespec *time0)
    {

        struct timespec diff = {.tv_sec = time1->tv_sec - time0->tv_sec, //
                                .tv_nsec = time1->tv_nsec - time0->tv_nsec};
        if (diff.tv_nsec < 0)
        {
            diff.tv_nsec += 1000000000; // nsec/sec
            diff.tv_sec--;
        }
        return diff;
    }
    timeval diff_timeval(const struct timeval *time1,
                         const struct timeval *time0)
    {

        timeval diff = {.tv_sec = time1->tv_sec - time0->tv_sec, //
                        .tv_usec = time1->tv_usec - time0->tv_usec};
        if (diff.tv_usec < 0)
        {
            diff.tv_usec += 1000000; // nsec/sec
            diff.tv_sec--;
        }
        return diff;
    }

    static struct
    {
        bool failed;
        timespec time;
    } out;
    int Pipe[2];

    void alarmHandler(int)
    {
        out.failed = true;
        out.time = {};
        write(Pipe[1], &out, sizeof(out));
        exit(-1);
    }

    void handleRegressionCalcs(regressionData *outR);

    struct regressionData regressionFinder(generatorFunction gf, functionTester ft, size_t OutputTypeSize,inputCleanup ic, int n)
    {
        regressionData outR{};
        char *res = new char[OutputTypeSize];
        for (size_t i = 1; i < n; i = i << 1) // n = max size to test (floor power of 2)
        {
            input in = gf(i);

            pipe(Pipe);
            pid_t id = fork();

            if (id == -1)
            {
                return outR;
            }
            else if (id == 0)
            {
                close(Pipe[0]);
                signal(SIGALRM, alarmHandler);
                alarm((int)maxTime);
                timespec start;
                timespec end;
 
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

                output out = ft(in, (void*)res);
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
                alarm(0);
                free(res);
                timespec diff = diff_timespec(&end, &start);
                printf("Time difference is %lds, %ldus\n", diff.tv_sec, diff.tv_nsec);
                bigO_Finder::out.time = diff;
                write(Pipe[1], &bigO_Finder::out, sizeof(bigO_Finder::out));
                close(Pipe[1]);
                exit(0);
            }
            else
            {
                close(Pipe[1]);
                read(Pipe[0], &out, sizeof(out));
                close(Pipe[1]);
                outR.outputPairs.push_back(std::make_pair(i, out.time));
            }
            ic(in);
        }
        delete[] res;
        handleRegressionCalcs(&outR);
        return outR;
    }

    /*REGRESSION HANDLING HELPERS*/
    static double timespecToNs(const timespec &ts)
    {
        // convert to timespec interval to nano seconds
        return 1e9 * static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec);
    }

    static double mse(const std::vector<double> &y, const std::vector<double> &yhat)
    {
        // given a runtime estimate model predictions (yhat) compute error from empircal runtimes
        double s = 0.0;
        for (size_t i = 0; i < y.size(); i++)
        {
            // sum error acrued from each input size
            double d = y[i] - yhat[i];
            s += d * d;
        }

        // average across all tests to get mean error
        return s / static_cast<double>(y.size());
    }

    /* ~~~ basis functions to test for our empirical regression estimation ~~~ */
    static double basis_constant(double n) { return 1.0; }
    static double basis_log(double n) { return std::log(std::max(2.0, n)); }
    static double basis_linear(double n) { return n; }
    static double basis_nlogn(double n) { return n * std::log(std::max(2.0, n)); }
    static double basis_quad(double n) { return n * n; }
    static double basis_cubic(double n) { return n * n * n; }
    // a* f(x) + b

    void handleRegressionCalcs(regressionData *outR)
    {
        // accumulate our function candidates into a vector of struct
        struct Candidate
        {
            const char *name;
            double (*basis)(double);
        };
        std::vector<Candidate> candidates = {
            {"O(1)", basis_constant},
            {"O(log n)", basis_log},
            {"O(n)", basis_linear},
            {"O(n log n)", basis_nlogn},
            {"O(n^2)", basis_quad},
            {"O(n^3)", basis_cubic}};

        // debug: print everything before regression (after parent recieves info from child)
        for (const auto& p : outR->outputPairs) {
            // printf("(n=%d, time_ns=%f)\n", p.first, timespecToNs(p.second));
            printf("(n=%d, time_s=%ld time_ns=%f)\n", p.first, p.second.tv_sec,timespecToNs(p.second));
        }

        // if less than 2 data points --> can't predict empirically, return error gracefully
        const size_t m = outR->outputPairs.size();
        if (m < 2)
        {
            outR->order = "Unknown";
            return;
        }

        // collect input sizes and resulting time intervals in nanoseconds
        // from our runs (as double)
        std::vector<double> ns(m), ts(m);
        for (size_t i = 0; i < m; i++)
        {
            ns[i] = static_cast<double>(outR->outputPairs[i].first); // sizes (x)
            ts[i] = timespecToNs(outR->outputPairs[i].second);       // run times (y)
        }

        // track current best MSE and function name
        double bestMSE = std::numeric_limits<double>::infinity();
        const char *bestName = "Unknown";

        // main loop: loop over each candidate function and calculate its MSE
        for (const auto &cand : candidates)
        {
            // use Eigen to manage f(input size) -> run time functions
            Eigen::MatrixXd X(m, 2);
            Eigen::VectorXd y(m);

            // load the input size and run time pairs into Eigen matrices
            for (size_t i = 0; i < m; i++)
            {
                X(i, 0) = cand.basis(ns[i]); // a * basis_function(x) + ...
                X(i, 1) = 1.0;               // ... + b
                y(i) = ts[i];
            }

            // solve the regression and use it to make predictions based on the input
            Eigen::VectorXd beta = X.colPivHouseholderQr().solve(y);
            Eigen::VectorXd yhat = X * beta;

            // store the predictions for this regression in a vector
            std::vector<double> pred(m);
            for (size_t i = 0; i < m; i++)
                pred[i] = yhat(i);

            // given predictions, ground truths -> compute the MSE from this model
            double modelMSE = mse(ts, pred);
            if (modelMSE < bestMSE)
            {
                // update bestMSE and O(f(x)) estimation is necessary
                bestMSE = modelMSE;
                bestName = cand.name;
            }
        }

        // result: O(F(x)) complexity estimation
        outR->order = bestName;
    }
}

namespace bigO_Finder2::Private
{
    void handleRegressionCalcs(regressionData *outR)
    {
        bigO_Finder::handleRegressionCalcs((bigO_Finder::regressionData *)outR);
    }
    void getTime(timespec* t)
    {

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, t);
    }
    timespec timeDiff(timespec *a, timespec *b)
    {
        return bigO_Finder::diff_timespec(a, b);
    }
    int pipe(int *a)
    {
        return ::pipe(a);
    }
    int fork()
    {
        return ::fork();
    }
    int close(int fd)
    {
        return ::close(fd);
    }
    using sighandler = void (*)(int);
    sighandler alarmSignalHandler(sighandler handler)
    {
        return signal(SIGALRM, handler);
    }
    int alarm(int time)
    {
        return ::alarm(time);
    }
    void alarmHandler(int a)
    {
        bigO_Finder::alarmHandler(a);
    }
    void readp(int f, void *b, size_t n)
    {
        read(f, b, n);
    }
    void writep(int f, const void *b, size_t n)
    {
        write(f, b, n);
    }
    void exit(int a)
    {
        ::exit(a);
    }

}
