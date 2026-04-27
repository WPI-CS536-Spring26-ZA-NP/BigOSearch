#include <map>
#include <tuple>
#include <vector>
#include <ctime>
#include <iostream>
namespace bigO_Finder
{

    inline double maxTime = 10.0; // Default Timeout is 10 secs but can be changed globally
    inline unsigned int maxJobs = 1;
    typedef bool (*EQFunc)(void *outputA, void *outputB);
    typedef void *(*functionTester)(void *input, void *result);
    typedef void *(*generatorFunction)(int size);
    typedef void *input;
    typedef void *output;
    typedef void *expectedOutput;
    typedef void (*inputCleanup)(input);
    typedef void (*outputCleanup)(output);
    std::tuple<bool, std::map<input, std::tuple<bool, output, expectedOutput>>> testingFunction(functionTester, EQFunc, std::map<input, expectedOutput>, size_t OutputTypeSize);
    void cleanUpTestResults(std::tuple<bool, std::map<input, std::tuple<bool, output, expectedOutput>>> result, outputCleanup);
    struct regressionData regressionFinder(generatorFunction, functionTester, size_t OutputTypeSize,inputCleanup,int);
    struct regressionData
    {
        const char *order;
        std::vector<std::pair<int, timespec>> outputPairs;
    };

};

namespace bigO_Finder2
{

    inline int maxTime = 10; // Default Timeout is 10 secs but can be changed globally
    // inline unsigned int maxJobs = 1;
    template <typename OutputType>
    using EQFunc = bool (*)(OutputType outputA, OutputType outputB);
    template <typename InputType, typename OutputType>
    using functionTester = OutputType (*)(InputType input);
    template <typename InputType>
    using generatorFunction = InputType (*)(int size);
    template <typename InputType, typename OutputType>
    std::tuple<bool, std::map<InputType, std::tuple<bool, OutputType, OutputType>>> testingFunction(functionTester<InputType, OutputType>, EQFunc<OutputType>, std::map<InputType, OutputType>);
    template <typename InputType, typename OutputType>
    struct regressionData regressionFinder(generatorFunction<InputType>, functionTester<InputType, OutputType>,int);
    struct regressionData
    {
        const char *order;
        std::vector<std::pair<int, timespec>> outputPairs;
    };

    template <typename InputType, typename OutputType>
    std::tuple<bool, std::map<InputType, std::tuple<bool, OutputType, OutputType>>>
    testingFunction(functionTester<InputType, OutputType> tester, EQFunc<OutputType> eqFunc, std::map<InputType, OutputType> map)
    {
        std::map<InputType, std::tuple<bool, OutputType, OutputType>> outmap;
        bool res = true;

        for (auto &&i : map)
        {

            OutputType oput = tester(i.first);
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

    namespace Private
    {
        void handleRegressionCalcs(regressionData *outR);
        void getTime(timespec *);
        timespec timeDiff(timespec *, timespec *);
        int pipe(int *);
        int fork();
        int close(int);
        using sighandler = void (*)(int);
        sighandler alarmSignalHandler(sighandler handler);
        int alarm(int time);
        void alarmHandler(int);
        void readp(int, void *, size_t);
        void writep(int, const void *, size_t);
        void exit(int);
        static inline struct {
            bool failed;
            timespec time;
        } out;
        static inline int Pipe[2];
    }

    template <typename InputType, typename OutputType>
    regressionData regressionFinder(generatorFunction<InputType> gf, functionTester<InputType, OutputType> ft,int n)
    {
        regressionData outR{};
        Private::pipe(Private::Pipe);
        for (size_t i = 1; i < n; i = i << 1)
        {
            
            pid_t id = Private::fork();

            if (id == -1)
            {
                return outR;
            }
            else if (id == 0)
            {
                InputType in = gf(i);
                std::cout << '.';
                Private::close(Private::Pipe[0]);
                Private::alarmSignalHandler(Private::alarmHandler);
                Private::alarm((int)maxTime);

                timespec start, end;
                Private::getTime(&start);
                // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
                OutputType outll = ft(in);
                Private::getTime(&end);
                // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
                Private::alarm(0);
                timespec diff = Private::timeDiff(&end, &start);

                Private::out.failed = false;
                Private::out.time = diff;
                Private::writep(Private::Pipe[1], &Private::out, sizeof(Private::out));
                Private::close(Private::Pipe[1]);
                Private::exit(0);
            }
            else
            {
                Private::readp(Private::Pipe[0], &Private::out, sizeof(Private::out));
                // time_t s,n;
                // s=Private::out.time.tv_sec;
                // n=Private::out.time.tv_nsec;
                // printf("%d, %ld, %ld.%ld",Private::out.failed, i,s,n);
                if(!Private::out.failed){
                    outR.outputPairs.push_back(std::make_pair(i, Private::out.time));
                }else
                {
                    break;
                }
            }
            
            
        }
        Private::close(Private::Pipe[1]);
        Private::close(Private::Pipe[0]);
        Private::handleRegressionCalcs(&outR);
        return outR;
    }

}