/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#import <set>
#import "test.h"
#import "datastructures.h"
#import "exceptions.h"
#import "utils.h"
#import "io.h"

using namespace mlnet;

void testIO() {
	log("TESTING IO (MLNetwork)");
	log("Reading network io1 from file...",false);
	MLNetworkSharedPtr mnet1 = read_multilayer("test/io1.mpx","mlnet 1",',');
	log("done! ",false);
	log(mnet1->to_string());

	log("Reading network io2 from file...",false);
	MLNetworkSharedPtr mnet2 = read_multilayer("test/io2.mpx","mlnet 2",',');
	log("done! ",false);
	log(mnet2->to_string());

	log("Reading network io3 from file...",false);
	MLNetworkSharedPtr mnet3 = read_multilayer("test/io3.mpx","mlnet 3",',');
	log("done! ",false);
	log(mnet3->to_string());

	log("Testing attribute values...",false);
	LayerSharedPtr layer = mnet3->get_layer("l1");
	NodeSharedPtr node = mnet3->get_node("U0",layer);
	if (mnet3->node_features(layer)->getString(node->id,"Color")!="Blue") throw FailedUnitTestException("Node string attribute not correctly read from file");
	if (mnet3->node_features(layer)->getNumeric(node->id,"Age")!=34) throw FailedUnitTestException("Node numeric attribute not correctly read from file");
	layer = mnet3->get_layer("l2");
	NodeSharedPtr node1 = mnet3->get_node("U1",layer);
	NodeSharedPtr node2 = mnet3->get_node("U3",layer);
	EdgeSharedPtr edge = mnet3->get_edge(node1,node2);
	if (mnet3->edge_features(layer,layer)->getNumeric(edge->id,"Weight")!=3) throw FailedUnitTestException("Edge weight not correctly read from file");
	if (mnet3->edge_features(layer,layer)->getNumeric(edge->id,"Stars")!=4) throw FailedUnitTestException("Edge numeric attribute not correctly read from file");
	log("done!");

	log("TEST SUCCESSFULLY COMPLETED (IO)");
}


