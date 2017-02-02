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

void test_io() {

	test_begin("IO");

	std::cout << "Reading network io1 from file...";
	MLNetworkSharedPtr mnet1 = read_multilayer("test/io1.mpx","mlnet 1",',');
	if (mnet1->get_actors()->size()!=5 ||
		mnet1->get_layers()->size()!=2 ||
		mnet1->get_nodes()->size()!=7 ||
		mnet1->get_edges()->size()!=7) throw FailedUnitTestException("Could not retrieve all network components");
	std::cout << "done! " << mnet1->to_string() << std::endl;

	std::cout << "Reading network io2 from file...";
	MLNetworkSharedPtr mnet2 = read_multilayer("test/io2.mpx","mlnet 2",',');
	if (mnet2->get_actors()->size()!=6 ||
		mnet2->get_layers()->size()!=2 ||
		mnet2->get_nodes()->size()!=8 ||
		mnet2->get_edges()->size()!=7) throw FailedUnitTestException("Could not retrieve all network components");
	std::cout << "done! " << mnet2->to_string() << std::endl;

	std::cout << "Reading network io3 from file...";
	MLNetworkSharedPtr mnet3 = read_multilayer("test/io3.mpx","mlnet 3",',');
	if (mnet3->get_actors()->size()!=7 ||
		mnet3->get_layers()->size()!=2 ||
		mnet3->get_nodes()->size()!=8 ||
		mnet3->get_edges()->size()!=7) throw FailedUnitTestException("Could not retrieve all network components");
	std::cout << "done! " << mnet3->to_string() << std::endl;

	std::cout << "Checking if attribute values have been retrieved...";
	LayerSharedPtr layer = mnet3->get_layer("l1");
	ActorSharedPtr actor0 = mnet3->get_actor("U0");
	ActorSharedPtr actor1 = mnet3->get_actor("U1");
	ActorSharedPtr actor3 = mnet3->get_actor("U3");
	NodeSharedPtr node = mnet3->get_node(actor0,layer);
	if (mnet3->node_features(layer)->getString(node->id,"Color")!="Blue") throw FailedUnitTestException("Node string attribute not correctly read from file");
	if (mnet3->node_features(layer)->getNumeric(node->id,"Age")!=34) throw FailedUnitTestException("Node numeric attribute not correctly read from file");
	layer = mnet3->get_layer("l2");
	NodeSharedPtr node1 = mnet3->get_node(actor1,layer);
	NodeSharedPtr node2 = mnet3->get_node(actor3,layer);
	EdgeSharedPtr edge = mnet3->get_edge(node1,node2);
	if (mnet3->edge_features(layer,layer)->getNumeric(edge->id,"Weight")!=3) throw FailedUnitTestException("Edge weight not correctly read from file");
	if (mnet3->edge_features(layer,layer)->getNumeric(edge->id,"Stars")!=4) throw FailedUnitTestException("Edge numeric attribute not correctly read from file");
	std::cout << "done!" << std::endl;

	std::cout << "Writing to file and reloading a multilayer network (native format)...";
	write_multilayer(mnet3,"_tmp_file1.mln",',');
	MLNetworkSharedPtr mnet4 = read_multilayer("_tmp_file1.mln","tmp",',');
	if (mnet4->get_actors()->size()!=7 ||
		mnet4->get_layers()->size()!=2 ||
		mnet4->get_nodes()->size()!=8 ||
		mnet4->get_edges()->size()!=7) throw FailedUnitTestException("Could not retrieve all network components");
	std::remove("_tmp_file1.mln");
	std::cout << "done!" << std::endl;

	std::cout << "Writing a graphml file...";
	hash_set<LayerSharedPtr> layers;
	for (LayerSharedPtr layer: *mnet3->get_layers())
		layers.insert(layer);
	write_graphml(mnet3,"_tmp_file2.gml",layers,true,true);
	std::remove("_tmp_file2.gml");
	write_graphml(mnet3,"_tmp_file3.gml",layers,false,false);
	std::remove("_tmp_file3.gml");
	std::cout << "done!" << std::endl;

	/*
	 * These functions can be checked by loading the files using another library,
	 * e.g., with igraph in R: g <- read.graph("pr2.gml",format="graphml"), and checking
	 * if all the information is preserved.
	 */
	//write_graphml(mnet3,"pr1.gml",layers,true,true);
	//write_graphml(mnet3,"pr2.gml",layers,true,false);
	//write_graphml(mnet3,"pr3.gml",layers,false,true);
	//write_graphml(mnet3,"pr4.gml",layers,false,false);

	test_end("IO");
}


