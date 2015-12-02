/*
 * Unit testing for module: datastructures
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 1.0
 */

#include "test.h"
#include "datastructures.h"

#include <iostream>

using namespace mlnet;

void test_datastructures() {
	/*
	 * This initial test is for components that are normally
	 * automatically created by the MLNetwork class. For an
	 * example of how to create and manipulate a MLNetwork
	 * see the next test.
	 */
	test_begin("basic MLNetwork components (node, edge, layer, actor)");
	std::cout << "Creating two actors...";
	ActorSharedPtr actor1(new actor(1,"Matteo"));
	ActorSharedPtr actor2(new actor(2,"Luca"));
	std::cout << "done!" << std::endl;
	std::cout << "Creating two layers...";
	LayerSharedPtr layer1(new layer(1,"Facebook"));
	LayerSharedPtr layer2(new layer(2,"Twitter"));
	std::cout << "done!" << std::endl;
	std::cout << "Creating four nodes...";
	NodeSharedPtr node1(new node(1,actor1,layer1));
	NodeSharedPtr node2(new node(2,actor1,layer2));
	NodeSharedPtr node3(new node(3,actor1,layer2));
	NodeSharedPtr node4(new node(4,actor2,layer1));
	std::cout << "done!" << std::endl;
	std::cout << "Creating five edges...";
	EdgeSharedPtr edge1(new edge(1,node1,node2,true)); // directed
	EdgeSharedPtr edge2(new edge(2,node2,node1,true)); // directed
	EdgeSharedPtr edge3(new edge(3,node1,node2,false)); // indirected
	EdgeSharedPtr edge4(new edge(3,node2,node1,false)); // indirected
	if (*edge1==*edge2) throw FailedUnitTestException("Wrong edge comparison");
	if (*edge2==*edge3) throw FailedUnitTestException("Wrong edge comparison");
	if (*edge3!=*edge4) throw FailedUnitTestException("Wrong edge comparison");
	std::cout << "done!" << std::endl;
	test_end("basic MLNetwork components (node, edge, layer, actor)");


	/*
	 * This test checks the main functionality of the MLNetwork class,
	 * and can be also used as an example of how to manipulate a MLNetwork
	 * created using this library.
	 */
	test_begin("MLNetwork - structure");
	std::cout << "Creating an empty ML network...";
	MLNetworkSharedPtr mnet = MLNetwork::create("friends");
	std::cout << mnet->to_string() << " done!" << std::endl;
	std::cout << "Adding three actors...";
	ActorSharedPtr a1 = mnet->add_actor("a1");
	ActorSharedPtr a2 = mnet->add_actor("a2");
	ActorSharedPtr a3 = mnet->add_actor("Matteo");
	if (mnet->add_actor("Matteo")) throw FailedUnitTestException("Duplicate actor insertion not caught");
	if (mnet->get_actor("Matteo") != a3) throw FailedUnitTestException("Could not retrieve actor");
	int num_actors=0;
	for (ActorSharedPtr actor : mnet->get_actors()) {
		num_actors++;
		std::cout << "A" << actor->id << " ";
	}
	if (num_actors!=3 || num_actors!=mnet->get_actors().size()) throw FailedUnitTestException("Could not retrieve all actors");
	std::cout << "done!" << std::endl;

	std::cout << "Adding three layers: ";
	LayerSharedPtr l1 = mnet->add_layer("l1",false);
	LayerSharedPtr l2 = mnet->add_layer("l2",false);
	LayerSharedPtr l3 = mnet->add_layer("Facebook",true);
	// should fail, that is, return NULL
	if (mnet->add_layer("Facebook",false)) throw FailedUnitTestException("Duplicate actor insertion not caught");
	mnet->set_directed(l1,l2,true);
	if (mnet->get_layer("Facebook") != l3) throw FailedUnitTestException("Could not retrieve layer");
	int num_layers=0;
	for (LayerSharedPtr layer : mnet->get_layers()) {
		num_layers++;
		std::cout << "L" << layer->id << " ";
	}
	if (num_layers!=3 || num_layers!=mnet->get_layers().size()) throw FailedUnitTestException("Could not retrieve all layers");
	std::cout << "done!" << std::endl;


	std::cout << "Adding 9 nodes: ";
	// Layer 1
	NodeSharedPtr n1v0 = mnet->add_node(a1,l1);
	NodeSharedPtr n1v1 = mnet->add_node(a2,l1);
	NodeSharedPtr n1v2 = mnet->add_node(a3,l1);
	// Layer 2
	NodeSharedPtr n2v0 = mnet->add_node(a1,l2);
	NodeSharedPtr n2v1 = mnet->add_node(a2,l2);
	NodeSharedPtr n2v2 = mnet->add_node(a3,l2);
	// Layer 3
	NodeSharedPtr n3v0 = mnet->add_node(a1,l3);
	NodeSharedPtr n3v1 = mnet->add_node(a2,l3);
	NodeSharedPtr n3v2 = mnet->add_node(a3,l3);

	int num_nodes=0;
	for (NodeSharedPtr node : mnet->get_nodes()) {
		num_nodes++;
		std::cout << "N" << node->id << " ";
	}
	if (num_nodes!=9 || num_nodes!=mnet->get_nodes().size()) throw FailedUnitTestException("Could not retrieve all nodes");
	std::cout << "done!" << std::endl;

	std::cout << "Adding five intra-layer edges and one inter-layer edge: ";
	EdgeSharedPtr e1 = mnet->add_edge(n1v0,n1v1);
	EdgeSharedPtr e2 = mnet->add_edge(n2v0,n2v1);
	EdgeSharedPtr e3 = mnet->add_edge(n2v1,n2v2);
	EdgeSharedPtr e4 = mnet->add_edge(n3v0,n3v2);
	EdgeSharedPtr e5 = mnet->add_edge(n3v2,n3v1);

	EdgeSharedPtr e6 = mnet->add_edge(n2v2,n3v1);

	int num_edges=0;
	for (EdgeSharedPtr edge : mnet->get_edges()) {
		num_edges++;
		std::cout << "N" << edge->v1->id << (edge->directed?"->":"--") << "N" << edge->v2->id << " ";
	}
	if (num_edges!=6 || num_edges!=mnet->get_edges().size()) throw FailedUnitTestException("Could not retrieve all nodes");
	std::cout << "done!" << std::endl;

	std::cout << "Getting in-neighbors: ";
	int num_neighbors=0;
	for (NodeSharedPtr node : mnet->neighbors(n3v2,IN)) {
		num_neighbors++;
		std::cout << "N" << node->id << " ";
	}
	if (num_neighbors!=1) throw FailedUnitTestException("Could not retrieve neighbors");
	std::cout << "done!" << std::endl;
	std::cout << "Getting out-neighbors: ";
	num_neighbors=0;
	for (NodeSharedPtr node : mnet->neighbors(n3v2,OUT)) {
		num_neighbors++;
		std::cout << "N" << node->id << " ";
	}
	if (num_neighbors!=1) throw FailedUnitTestException("Could not retrieve neighbors");
	std::cout << "done!" << std::endl;
	std::cout << "Getting in/out-neighbors: ";
	num_neighbors=0;
	for (NodeSharedPtr node : mnet->neighbors(n3v2,INOUT)) {
		num_neighbors++;
		std::cout << "N" << node->id << " ";
	}
	if (num_neighbors!=2) throw FailedUnitTestException("Could not retrieve neighbors");
	std::cout << "done!" << std::endl;
	std::cout << "Getting out-neighbors with undirected edges: ";
	num_neighbors=0;
	for (NodeSharedPtr node : mnet->neighbors(n2v1,OUT)) {
		num_neighbors++;
		std::cout << "N" << node->id << " ";
	}
	if (num_neighbors!=2) throw FailedUnitTestException("Could not retrieve neighbors");
	std::cout << "done!" << std::endl;
	std::cout << "Getting in/out-neighbors with undirected edges: ";
	num_neighbors=0;
	for (NodeSharedPtr node : mnet->neighbors(n2v1,INOUT)) {
		num_neighbors++;
		std::cout << "N" << node->id << " ";
	}
	if (num_neighbors!=2) throw FailedUnitTestException("Could not retrieve neighbors");
	std::cout << "done!" << std::endl;

	std::cout << "Erasing components: ";
	mnet->erase(n3v2);
	if (8 != mnet->get_nodes().size()) throw FailedUnitTestException("Could not retrieve all nodes");
	mnet->erase(e3);
	if (3 != mnet->get_edges().size()) throw FailedUnitTestException("Could not retrieve all edges");
	mnet->erase(a1);
	if (2 != mnet->get_actors().size()) throw FailedUnitTestException("Could not retrieve all actor");
	if (5 != mnet->get_nodes().size()) throw FailedUnitTestException("Could not retrieve all nodes");
	mnet->erase(l1);
	std::cout << mnet->to_string();
	if (2 != mnet->get_layers().size()) throw FailedUnitTestException("Could not retrieve all layers");
	if (3 != mnet->get_nodes().size()) throw FailedUnitTestException("Could not retrieve all nodes");
	std::cout << "done!" << std::endl;
	test_end("MLNetwork - structure");




	test_begin("MLNetwork - attribute management");
	mnet->node_features(l1)->add("weight",NUMERIC_TYPE);
	mnet->node_features(l1)->add("type",STRING_TYPE);
	mnet->edge_features(l1,l2)->add("weight",NUMERIC_TYPE);
	mnet->node_features(l1)->setNumeric(n1v0->id,"weight",32.4);
	mnet->node_features(l1)->setString(n1v0->id,"type","pro");
	if (mnet->node_features(l1)->getNumeric(n1v0->id,"weight")!=32.4) throw FailedUnitTestException("Could not retrieve previously set attribute");
	if (mnet->node_features(l1)->getString(n1v0->id,"type")!="pro") throw FailedUnitTestException("Could not retrieve previously set attribute");
	std::cout << "Attributes created for nodes on layer L" << l1->id << ":" << std::endl;
	for (AttributeSharedPtr attr: mnet->node_features(l1)->attributes()) {
		std::cout << "- Attribute \"" << attr->name() << "\", type: " << attr->type_as_string() << std::endl;
	}
	std::cout << "Attributes created for edges from layer L" << l1->id << " to layer L"  << l2->id << ":" << std::endl;
	for (AttributeSharedPtr attr: mnet->edge_features(l1,l2)->attributes()) {
		std::cout << "- Attribute \"" << attr->name() << "\", type: " << attr->type_as_string() << std::endl;
	}
	std::cout << "Attributes created for edges from layer L" << l2->id << " to layer L" << l1->id << " (none expected):" << std::endl;
	for (AttributeSharedPtr attr: mnet->edge_features(l2,l1)->attributes()) {
		std::cout << "- Attribute \"" << attr->name() << "\", type: " << attr->type_as_string() << std::endl;
	}
	std::cout << "done!" << std::endl;
	test_end("MLNetwork - attribute management");
}


