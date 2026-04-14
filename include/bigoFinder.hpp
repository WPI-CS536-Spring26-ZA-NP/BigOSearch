#include <map>
#include <tuple>
namespace bigO_Finder{

    inline double maxTime = 10.0; // Default Timeout is 10 secs but can be changed globally
    inline unsigned int maxJobs = 1;
    typedef bool (*EQFunc) (void* outputA, void* outputB);
    typedef void* (*functionTester) (void * input);
    typedef void* (*generatorFunction) (int size);
    typedef void* input;
    typedef void* output;
    typedef void* expectedOutput;
    std::tuple<bool, std::map<input, std::tuple<bool, output, expectedOutput>>> testingFunction (functionTester, EQFunc, std::map<input,expectedOutput>);
    struct regressionData regressionFinder(generatorFunction, functionTester);
    struct regressionData{};
};
