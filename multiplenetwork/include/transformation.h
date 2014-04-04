/*
 * transformation.h
 *
 *  Created on: Jun 11, 2013
 *      Author: magnanim
 */

#ifndef MULTIPLENETWORK_TRANSFORMATION_H_
#define MULTIPLENETWORK_TRANSFORMATION_H_

#include "datastructures.h"

typedef enum {MNET_OR_FLATTENING, MNET_WEIGHED_FLATTENING, MNET_MULTI_FLATTENING}
MNET_FLATTENING_ALGORITHM;

void flatten(MultipleNetwork& mnet, MNET_FLATTENING_ALGORITHM algorithm,
		Network& net);

#endif /* MULTIPLENETWORK_TRANSFORMATION_H_ */
