Brief description of the code structure of the multinet library

Author: Matteo Magnani <matteo.magnani@it.uu.se>
Version: 1.0

./include/multinet.h is the main header file, and lists all the
different modules in the library. This file can be useful to
get a quick overview of the main modules in the library.

Each module is made of:
- a header file (e.g., measures.h);
- a directory with all the source files (e.g., ./src/measures/)
- a file where the module's functions are tested (e.g., ./test/test_measures.cpp)

The test files are also useful to see examples of how the classes/methods in the
library can be used.

The ./test directory contains its own header file, as it is not necessarily
distributed together with the library, and the file test_all.cpp runs all the
tests so that after any updates it can be checked that no functionality has
been affected.

In the basic Makefile (Mac version) there are two targets, one to run the tests
(unit_test) and one to package the library in a library file to be used with
other programs. 