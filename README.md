multiplenetwork
===============

Analysis and mining of multiple(x) networks

The include directory contains all the classes/functions definitions and detailed comments. To use this as an external library, it is sufficient to include multiplenetwork.h (which just imports all other headers).

- datastructure.h contains the definitions of vertexes, edges, networks and multiple networks, in addition to paths and distances.
- measures.h contains the definition of the main analysis measures: degree, neoghborhood, network relevance, distance, betweenness, modularity, ... (all in their multiple-network extension).
- io.h contains functions to read and write multiple networks from/to file (so far only a simple read function has been implemented).
- transformation.h
- evolution.h
- community.h

The test directory contains unit-testing functions - every time a new polished function/class is added to the library a corresponding test function is added here. This is useful to understand how the library works. For example, looking at the code of testNetwork() we can see how a network is created and manipulated, etc. run_all (which is currently set as the default action in the Makefile) runs all the validated test functions. This should be tried every time the code is significantly changed.
