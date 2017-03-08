multinet
===============

A library for the analysis and mining of multilayer networks, based on the book: Multilayer Social Networks, by Dickison, Magnani and Rossi, Cambridge University Press. This is also available as an R library on CRAN.


Documentation
-------------

* A doxygen generated *API reference* which lists all types and functions
of the library.
* *Unit Tests* containing small code snippets used to test each library
feature.

Requirements
------------

The multinet library requires:

* A modern, C++11 ready compiler such as `g++` version 4.7 or higher or `clang` version 3.2 or higher.
* The *cmake* build system.

Installation
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

