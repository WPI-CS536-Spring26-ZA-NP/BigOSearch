#include <bigoFinder.hpp>
#include <climits>
#include <random>
#include <vector>
#include <cstdlib> // For rand(), srand()

using namespace std;
using namespace bigO_Finder;

int find_max(vector<int> v) {
	int biggest = INT_MIN;
	for (auto& val : v) {
		if (val > biggest) {
			biggest = val;
		}
	}
return biggest;
}


// src: https://www.geeksforgeeks.org/cpp/bubble-sort-in-cpp/
void bubble_sort(vector<int>& v) {
    int n = v.size();

    // Outer loop that corresponds to the number of
    // elements to be sorted
    for (int i = 0; i < n - 1; i++) {

        // Last i elements are already
        // in place
        for (int j = 0; j < n - i - 1; j++) {
          
          	// Comparing adjacent elements
            if (v[j] > v[j + 1])
              
              	// Swapping if in the wrong order
                swap(v[j], v[j + 1]);
        }
    }
}


int cubic_function(int num) {
	int result = 0;
	for (int i = 0; i < num; i++) {
		for (int j = 0; i < num; i++) {
			for (int k = 0; i < num; i++) {
				result += 1;
			}
		}	
	}
	return result;
}

bool int_equality(void* a, void* b) {
	return *(int*)(a) == *(int*)b;
}

bool vector_int_equality(void* aa, void* bb) {
	vector<int> a = *(vector<int>*)(aa);
	vector<int> b = *(vector<int>*)(bb);
	int size_a = a.size();
	int size_b = b.size();
	bool is_equal = (size_a == size_b);
	for (int i =0; i<size_a; i++) {
		is_equal = (is_equal && (a[i] == b[i]));
		if (!is_equal) {
			return false;
		}
	}
	return is_equal;
}

int int_input_generator(int size) {
	return size;
}

vector<int> vector_int_input_generator(int size) {
	vector<int> v = {};
	v.reserve(size);
	// Seed with current time to get different results each run
    	std::srand(static_cast<unsigned int>(size));

	for (int i = 0; i < size; i++) {
		v[i] = static_cast<int>(std::rand() * 67);
	}
 
	return v;
}

void* maxFunctionTester(void* input) {
	int* res = (int*)malloc(sizeof(int));
	*res = find_max(*static_cast<vector<int>*>(input));	
	return static_cast<void*>(res);	
}

void* bubbleSortFunctionTester(void* input) {
	bubble_sort(*static_cast<vector<int>*>(input));	
	return input;
}

void* cubicFunctionTester(void* input) {
	int* res = (int*)malloc(sizeof(int));
	*res = cubic_function(*static_cast<int*>(input));	
	return static_cast<void*>(res);	
}

int main(){
   // find max timing
   std::map<input, expectedOutput> findMaxMap;
   std::vector<int> list1 = {1, 2, 9, 3, 4, 5, 6, 7, 8};
   std::vector<int> list2 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
   std::vector<int> list3 = {9, 1, 2, 3, 4, 5, 6, 7, 8};
   std::vector<int> list4 = {1, 2, 3, 4, 5, 9, 6, 7, 8};
   int maxresult = 9;
   findMaxMap[(void*)&list1] = (void*)&maxresult;
   findMaxMap[(void*)&list2] = (void*)&maxresult;	
   findMaxMap[(void*)&list3] = (void*)&maxresult;	
   findMaxMap[(void*)&list4] = (void*)&maxresult;	
   auto findMaxResult = testingFunction(maxFunctionTester, int_equality, findMaxMap);
   printf("Result of Max testing (all):%d\n", std::get<0>(findMaxResult));
}
