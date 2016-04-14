/*
 * mlnetwork.h
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 1.0
 *
 * General header file, including all the modules of the library.
 *
 * Each consolidated header file contains the documentation (comments)
 * of all functions, classes, etc. Source files only contain comments
 * concerning implementation details.
 *
 */

#ifndef MLNET_MLNETWORK_H_
#define MLNET_MLNETWORK_H_

// Exceptions:
#include "exceptions.h"
// Various utility functions/classes (like CSV reading, support data structures...):
#include "utils.h"
// Main data structures, in particular the MLNetwork class to represent multilayer networks:
#include "datastructures.h"
// Reading and writing network data:
#include "io.h"
// Main analytical measures (degree, neighborhood, relevance, ...):
#include "measures.h"
// Generation of multilayer networks:
#include "generation.h"
// Functions for community detection:
#include "community.h"
// Random walks on multilayer networks:
#include "randomwalks.h"
// Network pre-processing (flattening, projection...):
#include "transformation.h"
// Dynamical processes (viral models, threshold models...):
#include "dynamics.h"

#endif /* MLNET_MLNETWORK_H_ */
