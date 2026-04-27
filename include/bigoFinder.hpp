#include <map>
#include <tuple>
#include <vector>
#include <time.h>
namespace bigO_Finder
{

    inline double maxTime = 10.0; // Default Timeout is 10 secs but can be changed globally
    inline unsigned int maxJobs = 1; // currently unused
// aliases for our types

// takes in two outputs and returns if they are equal
// provided by user
    typedef bool (*EQFunc)(void *outputA, void *outputB);

// a wrapper around the function you want to call that lifts input
// into the correct type before passing it into your function and
// returns the result in place of the result param
// provided by user
    typedef void *(*functionTester)(void *input, void *result); 
//type aliases for input and output
    typedef void *input;
    typedef void *output;
    typedef void *expectedOutput; // -- just here for to be explicit
// generates Input given size
// provided by user
    typedef void *(*generatorFunction)(int size);
// pass to regressionFinder to destroy generated input when done using it
    typedef void (*inputCleanup)(input);
// pass to clean up test results to destroy output data when done
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
