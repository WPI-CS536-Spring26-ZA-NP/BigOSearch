#include "bigoFinder.hpp"
#include <unistd.h>
#include <thread>
#include <mutex>

namespace bigO_Finder {

    std::tuple<bool, expectedOutput>* mailbox = nullptr;
    std::thread* threads = nullptr;

    std::tuple<bool, std::map<input, std::tuple<bool, output, expectedOutput>>>
        testingFunction(functionTester tester, EQFunc eqFunc, std::map<input, expectedOutput> map) {

        std::map<input, std::tuple<bool, output, expectedOutput>> outmap;
        bool res = true;

        for (auto&& i : map) {

            output oput = tester(i.first);
            bool valid = eqFunc(oput, i.second);
            outmap[i.first] = std::tuple(valid, oput, i.second);
            if (!valid) {
                res = false;
                // break
            }
        }
        return std::tuple(res, outmap);
    }
