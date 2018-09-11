multinet
===============

NOTE: This version is no longer under development. We are in the process of releasing version 2.0, that will mantain (almost) the same API for the R package but with a re-engineered C++ code. We will soon post here the address of the new repository.

A library for the analysis and mining of multilayer networks, based on the book: Multilayer Social Networks, by Dickison, Magnani and Rossi, Cambridge University Press. This is available as a C++ library and as an R library. A stable version of the R library is also available on CRAN.


Documentation
-------------

* A doxygen generated *API reference* which lists all types and functions
of the library.
* [Unit Tests](test/) which contain small code snippets used to test each
library feature.

Requirements
------------

The multinet library requires:

* A modern, C++11 ready compiler such as `g++` version 4.7 or higher or `clang` version 3.2 or higher.
* The *cmake* build system.

Installation - C++
------------

To download and install the library use the following commands.

```sh
git clone https://github.com/magnanim/multinet.git
cd multinet

mkdir build
cd build

cmake ..
make
```

This compiles and generates a sahred library into the `build` directory in the
root library. A different location can be specified by executing cmake from
another folder.

The documentation is generated inside the `build/doc` directory, while the
testing suite will reside in `build/test`. In order run the test suite execute:

```sh
cd multinet/build/test
./multinet-tests
```

To install the library in your system use the generated Makefile script:

```sh
cd multinet/build
make install
```

Installation - R
------------
The stable version of the R library can be installed directly from R by typing:

```sh
install.packages("multinet")
```

To install the latest version you should chdir to Rmultinet, create a /src folder and copy into it:
1. all the .c, .cpp and .h files from the C++ library (../multinet)
2. all the files from the C++ directory
3. all the files from the R directory

Then: 
```sh
R CMD build Rmultinet
R CMD check multinet_1.1.tar.gz
```
