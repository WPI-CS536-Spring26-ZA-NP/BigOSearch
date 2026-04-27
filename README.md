# BigOSearch

There are many solutions for testing the correctness of algorithms, and there are also many solutions for testing performance growth. However, there is an underexplored gap that combines the two. As such, we introduce a new framework that explicitly encodes run-time estimation and correctness verification operations. Our framework, built in C/C++, provides an API to the user that includes two major components: (1) a correctness verification function, and (2) a time complexity analysis estimator. These functions constitute the major functionality of our framework that can be utilized by software testers to better test their code.

To build the project, we use CMake. The following commands can be used to build the project:

`cmake -S . -B build`

`cmake --build build`

To run our current pipeline, which demonstrates the framework on the find_max, bubble_sort, and cubic_function functions, run:

`./build/Tests`
