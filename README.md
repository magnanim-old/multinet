mlnetwork
===============

A library for the analysis and mining of multilayer networks, based on the forthcoming book: Multilayer Social Networks, by Dickison, Magnani and Rossi, Cambridge University Press. This is also available as an R library on CRAN.


Documentation
-------------

We provide a set of documentation describing the data structures and algorithms
provided by the library. More specifically, we provide:

* An doxygen generated *API reference* which lists all types and functions
of the library.
* [Unit Tests](test/) which contain small code snippets used to test each
library feature.

Requirements
------------

The mlnetwork library requires:

* A modern, C++11 ready compiler such as `g++` version 4.7 or higher or `clang` version 3.2 or higher.
* The [cmake][cmake] build system.
* A 64-bit operating system. Either Mac OS X or Linux are currently supported.

Installation
------------

To download and install the library use the following commands.

``sh
git clone https://github.com/magnanim/multinet.git
cd multinet

mkdir build
cd build

cmake ..
make
``

This compiles and generates a sahred library into the `build` directory in the
root library. A different location can be specified by executing cmake from
another folder. 

The documentation is generated inside the `build/doc` directory, while the
testing suite will reside in `build/test`. In order run the test suite execute:

``sh
cd multinet/<build_directory>/test
./mlnetwork-tests
``

To install the library in your system use the generated Makefile script:

``sh
cd multinet/<build_directory>
make install
``

Getting Started
------------`
