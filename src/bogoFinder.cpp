#include "bigoFinder.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <thread>
#include <mutex>

namespace bigO_Finder
{

    std::tuple<bool, expectedOutput> *mailbox = nullptr;
    std::thread *threads = nullptr;

    std::tuple<bool, std::map<input, std::tuple<bool, output, expectedOutput>>>
    testingFunction(functionTester tester, EQFunc eqFunc, std::map<input, expectedOutput> map)
    {

        std::map<input, std::tuple<bool, output, expectedOutput>> outmap;
        bool res = true;

        for (auto &&i : map)
        {

            output oput = tester(i.first);
            bool valid = eqFunc(oput, i.second);
            outmap[i.first] = std::tuple(valid, oput, i.second);
            if (!valid)
            {
                res = false;
                // break
            }
        }
        return std::tuple(res, outmap);
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

    struct regressionData regressionFinder(generatorFunction gf, functionTester ft)
    {
        regressionData outR{};
        for (size_t i = 1; i < 10000; i = i << 2)
        {
            input in = gf(i);

            pid_t id = fork();
            pipe(Pipe);

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
                output out = ft(in);
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
                alarm(0);
                timespec diff = diff_timespec(&end, &start);
                bigO_Finder::out.failed = false;
                bigO_Finder::out.time = diff;
                write(Pipe[1], &out, sizeof(out));
                exit(0);
            }
            else
            {

                read(Pipe[0], &out, sizeof(out));
                outR.outputPairs.push_back(std::make_pair(i, out.time));
            }
        }
        handleRegressionCalcs(&outR);

        return outR;
    }

    void handleRegressionCalcs(regressionData *outR)
    {
    }
}
