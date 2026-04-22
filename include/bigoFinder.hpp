#include <map>
#include <tuple>
#include <vector>
#include <time.h>
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
