#include <bigoFinder.hpp>
#include <climits>
#include <random>
#include <vector>
#include <cstdlib> // For rand(), srand()

using namespace std;
using namespace bigO_Finder;

int find_max(vector<int> v)
{
    int biggest = INT_MIN;
    for (auto &val : v)
    {
        if (val > biggest)
        {
            biggest = val;
        }
    }
    return biggest;
}

// src: https://www.geeksforgeeks.org/cpp/bubble-sort-in-cpp/
void bubble_sort(vector<int> &v)
{
    int n = v.size();
    // Outer loop that corresponds to the number of
    // elements to be sorted
    for (int i = 0; i < n - 1; i++)
    {

        // Last i elements are already
        // in place
        for (int j = 0; j < n - i - 1; j++)
        {

            // Comparing adjacent elements
            if (v[j] > v[j + 1])

                // Swapping if in the wrong order
                swap(v[j], v[j + 1]);
        }
    }
}

int cubic_function(int num)
{
    int result = 0;
    for (int i = 0; i < num; i++)
    {
        for (int j = 0; j < num; j++)
        {
            for (int k = 0; k < num; k++)
            {
                result += 1;
            }
        }
    }
    return result;
}

bool int_equality(void *a, void *b)
{
    return *(int *)(a) == *(int *)b;
}

bool vector_int_equality(void *aa, void *bb)
{
    vector<int> a = *(vector<int> *)(aa);
    vector<int> b = *(vector<int> *)(bb);
    int size_a = a.size();
    int size_b = b.size();
    bool is_equal = (size_a == size_b);
    for (int i = 0; i < size_a; i++)
    {
        is_equal = (is_equal && (a[i] == b[i]));
        if (!is_equal)
        {
            return false;
        }
    }
    return is_equal;
}

void *int_input_generator(int size)
{
    int *v = new int;
    *v = size;
    return (void *)v;
}

void int_input_cleanup_function(input in)
{
    delete (int *)in;
}

void *vector_int_input_generator(int size)
{
    vector<int> *v = new vector<int>();

    // bubble_sort(*static_cast<vector<int> *>(input));

    v->resize(size);
    // Seed with current time to get different results each run
    std::srand(static_cast<unsigned int>(size));

    for (int i = 0; i < size; i++)
    {
        (*v)[i] = static_cast<int>(std::rand() % 67);
    }

    return v;
}

vector<int> vector_int_input_generator2(int size)
{
    vector<int> v;

    // bubble_sort(*static_cast<vector<int> *>(input));

    v.resize(size);
    // Seed with current time to get different results each run
    std::srand(static_cast<unsigned int>(size));

    for (int i = 0; i < size; i++)
    {
        v[i] = static_cast<int>(std::rand() * 67);
    }

    return v;
}

void vector_int_cleanup_function(input in)
{
    delete (vector<int> *)in;
}

void *maxFunctionTester(void *input, void *res)
{
    int *val = (int *)res;
    *val = find_max(*static_cast<vector<int> *>(input));
    return static_cast<void *>(val);
}

void *bubbleSortFunctionTester(void *input, void *res)
{
    // vector<int> *val = (vector<int> *)res;
    vector<int> *val = new (res) vector<int>();

    // bubble_sort(*static_cast<vector<int> *>(input));
    vector<int> *inVec = static_cast<vector<int> *>(input);
    bubble_sort(*inVec);
    // *val = *static_cast<vector<int> *>(input);
    *val = *inVec;
    return static_cast<void *>(val);
}

void *cubicFunctionTester(void *input, void *res)
{
    int *val = (int *)res;
    *val = cubic_function(*static_cast<int *>(input));
    return static_cast<void *>(val);
}

int main()
{

    {
        std::map<int, int> cubMap;
        cubMap[1] = 1;
        cubMap[2] = 8;
        cubMap[3] = 27;
        cubMap[9] = 729;
        cubMap[100] = 1000000;
        auto findCubRest = bigO_Finder2::testingFunction<int, int>(cubic_function, [](int a, int b)
                                                                   { return a == b; }, cubMap);
        printf("Result of Cub testing (all):%d\n", std::get<0>(findCubRest));
        for (auto &&i : std::get<1>(findCubRest))
        {
            printf("Result of Cub instance testing (%d):%d -- got %d\texpected %d\n", i.first, std::get<0>(i.second), std::get<1>(i.second), std::get<2>(i.second));
        }

        auto n = 2048;
        struct bigO_Finder2::regressionData bubbleSortTimeResult = bigO_Finder2::regressionFinder<int, int>([](auto a)
                                                                                                            { return a; }, [](auto a)
                                                                                                            { return cubic_function(a); }, n);
        printf("Calculated cubing runtime: %s\n", bubbleSortTimeResult.order);

    }
    {
        // bubbleSort timing

        std::map<std::vector<int>, std::vector<int>> bubbleSortMap;
        std::vector<int> list1_b = {1, 2, 9, 3, 4, 5, 6, 7, 8};
        std::vector<int> list2_b = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::vector<int> list3_b = {9, 1, 2, 3, 4, 5, 6, 7, 8};
        std::vector<int> list4_b = {1, 2, 3, 4, 5, 9, 6, 7, 8};
        std::vector<int> buubleresult = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        bubbleSortMap[list1_b] = buubleresult;
        bubbleSortMap[list2_b] = buubleresult;
        bubbleSortMap[list3_b] = buubleresult;
        bubbleSortMap[list4_b] = buubleresult;
        auto findBuubleResult = bigO_Finder2::testingFunction<std::vector<int>, std::vector<int>>([](auto a)
                                                                                                  { bubble_sort(a); return a; }, [](auto a, auto b)
                                                                                                  { return a == b; }, bubbleSortMap);
        printf("Result of Bubble testing (all):%d\n", std::get<0>(findBuubleResult));
        // for (auto &&i : std::get<1>(findBuubleResult))
        // {
        //     printf("Result of Max testing (%d):%d -- got %d\texpected %d\n", i.first, std::get<0>(i.second), std::get<1>(i.second), std::get<2>(i.second));
        // }

        auto n = 10000;
        struct bigO_Finder2::regressionData bubbleSortTimeResult = bigO_Finder2::regressionFinder<std::vector<int>, std::vector<int>>(vector_int_input_generator2, [](auto a)
                                                                                                                                      { bubble_sort(a); return a; },n);
        printf("Calculated bubble sort runtime: %s\n", bubbleSortTimeResult.order);
    }
    {
        // findMax timing

        std::map<std::vector<int>, int> findMaxMap;

        std::vector<int> list1 = {1, 2, 9, 3, 4, 5, 6, 7, 8};
        std::vector<int> list2 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::vector<int> list3 = {9, 1, 2, 3, 4, 5, 6, 7, 8};
        std::vector<int> list4 = {1, 2, 3, 4, 5, 9, 6, 7, 8};

        findMaxMap[list1] = 9;
        findMaxMap[list2] = 9;
        findMaxMap[list3] = 9;
        findMaxMap[list4] = 9;

        auto findMaxResult =
            bigO_Finder2::testingFunction<std::vector<int>, int>(
                [](std::vector<int> a) {
                    return find_max(a);
                },
                [](int a, int b) {
                    return a == b;
                },
                findMaxMap
            );

        printf("Result of find_max testing (all):%d\n", std::get<0>(findMaxResult));

        auto n = 100000;

        struct bigO_Finder2::regressionData findMaxTimeResult =
            bigO_Finder2::regressionFinder<std::vector<int>, int>(
                vector_int_input_generator2,
                [](std::vector<int> a) {
                    return find_max(a);
                },
                n
            );

        printf("Calculated find_max runtime: %s\n", findMaxTimeResult.order);
    }
}
