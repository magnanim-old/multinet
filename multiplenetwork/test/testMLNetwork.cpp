/*
 * testMultilayerNetwork.cpp
 *
 * Author: Matteo Magnani <matteo.magnani@it.uu.se>
 * Version: 0.0.1
 */

#import "test.h"
#import "datastructures.h"
#import "exceptions.h"
#import "utils.h"

using namespace mlnet;

void testMLNetwork() {
	log("TESTING basic MLNetwork components (node, edge, layer, actor)",true);
	// These are normally automatically created by functions of the MultilayerNetwork class.
	log("...creating two actors...",false);
	ActorSharedPtr actor1(new actor(1,"Matteo"));
	ActorSharedPtr actor2(new actor(2,"Luca"));
	log("done!");
	log("...creating two layers...",false);
	LayerSharedPtr layer1(new layer(1,"Facebook"));
	LayerSharedPtr layer2(new layer(2,"Twitter"));
	log("done!");
	log("...creating four nodes...",false);
	NodeSharedPtr node1(new node(1,"Matteo Magnani",actor1,layer1));
	NodeSharedPtr node2(new node(2,"mmagnani",actor1,layer2));
	NodeSharedPtr node3(new node(3,"matmagnani",actor1,layer2));
	NodeSharedPtr node4(new node(4,"Luca Rossi",actor2,layer1));
	log("done!");
	log("...creating five edges...",false);
	EdgeSharedPtr e1(new edge(1,node1,node2,true)); // directed
	EdgeSharedPtr e2(new edge(2,node2,node1,true)); // directed
	EdgeSharedPtr e3(new edge(3,node1,node2,false)); // indirected
	EdgeSharedPtr e4(new edge(3,node2,node1,false)); // indirected
	if (*e1==*e2) throw FailedUnitTestException("Wrong edge comparison");
	if (*e2==*e3) throw FailedUnitTestException("Wrong edge comparison");
	if (*e3!=*e4) throw FailedUnitTestException("Wrong edge comparison");
	log("done!");
	log("TEST SUCCESSFULLY COMPLETED (basic MLNetwork components)");



	log("******************************************");
	log("TESTING MLNetwork");
	log("Creating an empty ML network...",false);
	MLNetworkSharedPtr mnet = MLNetwork::create("friends");
	log(mnet->to_string() + " done!");
	log("Adding three actors...",false);
	ActorSharedPtr a1 = mnet->add_actor(); // new ID and name automatically assigned
	ActorSharedPtr a2 = mnet->add_actor(); // new ID and name automatically assigned
	ActorSharedPtr a3 = mnet->add_actor("Matteo"); // new ID automatically assigned, name "Matteo"
	//if ((*mnet->getActor(3))!=(*a2)) throw FailedUnitTestException("Could not retrieve actor");
	if ((*mnet->get_actor("Matteo"))!=(*a3)) throw FailedUnitTestException("Could not retrieve actor");
	actor_list actors = mnet->get_actors();
	int num_actors=0;
	for (ActorSharedPtr actor : actors) {
		num_actors++;
		log(actor->to_string() + " ",false);
	}
	if (num_actors!=3 || num_actors!=mnet->num_actors()) throw FailedUnitTestException("Could not retrieve all actors");
	log("done!");
	log("Adding duplicate actors (should fail)...",false);
	// the following instruction should generate an error: the actor already exists
	try {
		mnet->add_actor("Matteo");
		// should not arrive here
		throw FailedUnitTestException("Duplicate actor insertion not caught");
	}
	catch (DuplicateElementException& ex) {
		log("[FAIL] ",false);
	}
	log("done!");

	log("Adding three layers: ",false);
	LayerSharedPtr l1 = mnet->add_layer(false);
	LayerSharedPtr l2 = mnet->add_layer(false);
	LayerSharedPtr l3 = mnet->add_layer("Facebook",true);
	mnet->set_directed(l2,l3,true);
	if ((*mnet->get_layer("Facebook"))!=(*l3)) throw FailedUnitTestException("Could not retrieve layer");
	layer_list layers = mnet->get_layers();
	int num_layers=0;
	for (LayerSharedPtr layer : layers) {
		num_layers++;
		log(layer->to_string() + " ",false);
	}
	if (num_layers!=3 || num_layers!=mnet->num_layers()) throw FailedUnitTestException("Could not retrieve all layers");
	log("");
	log("done!");

	// the following instruction should fail: the layer already exists
	try {
		mnet->add_layer("Facebook",false);
		// should not arrive here
		throw FailedUnitTestException("Duplicate layer insertion not caught");
	}
	catch (DuplicateElementException& ex) {
		log("[FAIL] ",false);
	}
	log("done!");

	log("Adding 9 nodes: ",false);
	// Layer 1
	NodeSharedPtr n1v0 = mnet->add_node("v0",a1,l1);
	NodeSharedPtr n1v1 = mnet->add_node("v1",a2,l1);
	NodeSharedPtr n1v2 = mnet->add_node("v2",a3,l1);
	// Layer 2
	NodeSharedPtr n2v0 = mnet->add_node("v0",a1,l2);
	NodeSharedPtr n2v1 = mnet->add_node("v1",a2,l2);
	NodeSharedPtr n2v2 = mnet->add_node("v2",a3,l2);
	// Layer 3
	NodeSharedPtr n3v0 = mnet->add_node("v0",a1,l3);
	NodeSharedPtr n3v1 = mnet->add_node("v1",a2,l3);
	NodeSharedPtr n3v2 = mnet->add_node("v2",a3,l3);

	node_list nodes = mnet->get_nodes();
	int num_nodes=0;
	for (NodeSharedPtr node : nodes) {
		num_nodes++;
		log(node->to_string() + " ",false);
	}
	if (num_nodes!=9 || num_nodes!=mnet->num_nodes()) throw FailedUnitTestException("Could not retrieve all nodes");
	log("");
	log("done!");

	log("Adding five intra-layer edges and one inter-layer edge: ",false);
	mnet->add_edge(n1v0,n1v1);
	mnet->add_edge(n2v0,n2v1);
	mnet->add_edge(n2v1,n2v2);
	mnet->add_edge(n3v0,n3v2);
	mnet->add_edge(n3v2,n3v1);

	mnet->add_edge(n2v2,n3v1);

	edge_list edges = mnet->get_edges();
	int num_edges=0;
	for (EdgeSharedPtr edge : edges) {
		num_edges++;
		log(edge->to_string() + " ",false);
	}
	if (num_edges!=6 || num_nodes!=mnet->num_nodes()) throw FailedUnitTestException("Could not retrieve all nodes");
	log("done!");

	log("TESTING attribute management");

	mnet->node_features(l1)->add("weight",NUMERIC_TYPE);
	mnet->node_features(l1)->add("type",STRING_TYPE);
	mnet->edge_features(l1,l2)->add("weight",NUMERIC_TYPE);
	mnet->node_features(l1)->setNumeric(n1v0->id,"weight",32.4);
	mnet->node_features(l1)->setString(n1v0->id,"type","pro");
	if (mnet->node_features(l1)->getNumeric(n1v0->id,"weight")!=32.4) throw FailedUnitTestException("Could not retrieve previously set attribute");
	if (mnet->node_features(l1)->getString(n1v0->id,"type")!="pro") throw FailedUnitTestException("Could not retrieve previously set attribute");
	log("Attributes created for nodes on layer " + l1->to_string() + ":");
	for (AttributeSharedPtr attr: mnet->node_features(l1)->attributes()) {
		log("- Attribute \"" + attr->name()+ "\", type: " + attr->type_as_string());
	}
	log("Attributes created for edges from layer " + l1->to_string() + " to layer "  + l2->to_string() + ":");
	for (AttributeSharedPtr attr: mnet->edge_features(l1,l2)->attributes()) {
		log("- Attribute \"" + attr->name()+ "\", type: " + attr->type_as_string());
	}
	log("Attributes created for edges from layer " + l2->to_string() + " to layer "  + l1->to_string() + " (none expected):");
	for (AttributeSharedPtr attr: mnet->edge_features(l2,l1)->attributes()) {
		log("- Attribute \"" + attr->name()+ "\", type: " + attr->type_as_string());
	}
	log("done!");

	log("Getting in-neighbors: ",false);
	node_list neigh_in = mnet->neighbors(n3v2,IN);
	int num_neighbors=0;
	for (NodeSharedPtr node : neigh_in) {
		num_neighbors++;
		log(node->to_string() + " ",false);
	}
	if (num_neighbors!=1) throw FailedUnitTestException("Could not retrieve neighbors");
	log("");
	log("Getting out-neighbors: ",false);
	node_list neigh_out = mnet->neighbors(n3v2,OUT);
	num_neighbors=0;
	for (NodeSharedPtr node : neigh_out) {
		num_neighbors++;
		log(node->to_string() + " ",false);
	}
	if (num_neighbors!=1) throw FailedUnitTestException("Could not retrieve neighbors");
	log("done!");
	log("Getting in/out-neighbors: ",false);
	node_list neigh_all = mnet->neighbors(n3v2,INOUT);
	num_neighbors=0;
	for (NodeSharedPtr node : neigh_all) {
		num_neighbors++;
		log(node->to_string() + " ",false);
	}
	if (num_neighbors!=2) throw FailedUnitTestException("Could not retrieve neighbors");
	log("done!");
	log("Getting out-neighbors with undirected edges: ",false);
	node_list neigh_out_undirected = mnet->neighbors(n2v1,OUT);
	num_neighbors=0;
	for (NodeSharedPtr node : neigh_out_undirected) {
		num_neighbors++;
		log(node->to_string() + " ",false);
	}
	if (num_neighbors!=2) throw FailedUnitTestException("Could not retrieve neighbors");
	log("done!");
	log("Getting in/out-neighbors with undirected edges: ",false);
	node_list neigh_all_undirected = mnet->neighbors(n2v1,INOUT);
	num_neighbors=0;
	for (NodeSharedPtr node : neigh_all_undirected) {
		num_neighbors++;
		log(node->to_string() + " ",false);
	}
	if (num_neighbors!=2) throw FailedUnitTestException("Could not retrieve neighbors");
	log("done!");

	log("Printing final multiple network information: " + mnet->to_string());
	print(mnet);
	log("TEST SUCCESSFULLY COMPLETED (MLNetwork)");
}


