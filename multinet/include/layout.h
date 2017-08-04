/**
 * This module defines ...
 */

#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "datastructures.h"

namespace mlnet {
    
    struct xyz_coordinates {
    public:
        double x;
        double y;
        double z;
    };
    
    hash_map<NodeSharedPtr,xyz_coordinates> circular(MLNetworkSharedPtr& mnet, double radius);
    
    double fr(double p, double k);
    
    double fain(double p, double k);
    
    double fainter(double p, double k);
    
    hash_map<NodeSharedPtr,xyz_coordinates> multiforce(MLNetworkSharedPtr& mnet, double width, double length, const hash_map<LayerSharedPtr,double>& weight_in, const hash_map<LayerSharedPtr,double>& weight_inter, const hash_map<LayerSharedPtr,double>& gravity, int iterations);
    
    double layout_eval_internal(const MLNetworkSharedPtr& mnet, const hash_map<NodeSharedPtr,xyz_coordinates>& pos, double width, double length);
    
    double layout_eval_external(const MLNetworkSharedPtr& mnet, const hash_map<NodeSharedPtr,xyz_coordinates>& pos, double width, double length);
}

#endif /* LAYOUT_H_ */
