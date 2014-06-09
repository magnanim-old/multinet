/*
 * testNetwork.cpp
 *
 * Created on: Feb 7, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#import "test.h"
#import "datastructures.h"
#import "exceptions.h"
#import "utils.h"

void testNetwork() {
	log("**************************************************************");
	log("TESTING basic network components (vertex_id, edge_id)...",false);
	/*
	 * vertex and edge identifiers are normally automatically created
	 * by functions of the Network class. However, here we test them by
	 * directly manipulating them.
	 */
	vertex_id vid1 = 0;
	vertex_id vid2 = 1;
	// a directed edge
	edge_id e1(vid1,vid2,true);
	// another directed edge
	edge_id e2(vid2,vid1,true);
	// a third directed edge
	edge_id e3(vid2,vid1,true);
	// an undirected edge
	edge_id e4(vid1,vid2,false);
	// another undirected edge
	edge_id e5(vid2,vid1,false);
	if (e1==e2) throw FailedUnitTestException("Wrong edge_id comparison");
	if (e2!=e3) throw FailedUnitTestException("Wrong edge_id comparison");
	if (e4!=e5) throw FailedUnitTestException("Wrong edge_id comparison");
	log("done!");

	/**********************************************************************/
	log("**************************************************************");
	log("TESTING Network class (undirected, unweighed, unnamed)");
	log("Creating the network...",false);
	Network uuu_net(false,false,false);
	log("done!");

	log("Adding four vertexes to the network...",false);
	vertex_id vd0 = uuu_net.addVertex();
	vertex_id vd1 = uuu_net.addVertex();
	vertex_id vd2 = uuu_net.addVertex();
	vertex_id vd3 = uuu_net.addVertex();
	if (uuu_net.getNumVertexes()!=4) throw FailedUnitTestException("Wrong number of vertexes: " + std::to_string(uuu_net.getNumVertexes()) + " instead of 4");
	log("done!");

	log("Trying to add a named vertex (should fail)...",false);
	try {
		uuu_net.addVertex("an identifier");
		throw FailedUnitTestException("Unsupported operation (add named vertex) was allowed");
	}
	catch (OperationNotSupportedException& ex) {
		log("[FAIL] done!");
	}

	log("Adding four edges...",false);
	uuu_net.addEdge(vd0,vd1);
	uuu_net.addEdge(vd1,vd1);
	uuu_net.addEdge(vd1,vd2);
	uuu_net.addEdge(vd2,vd2);
	if (uuu_net.getNumEdges()!=4) throw FailedUnitTestException("Wrong number of edges");
	log("done!");

	log("Checking containsVertex function...",false);
	if (!uuu_net.containsVertex(vd3)) throw FailedUnitTestException("Vertex inserted, but containsVertex returns false");
	if (uuu_net.containsVertex(vd3+1)) throw FailedUnitTestException("Vertex not present, but containsVertex returns true");
	log("done!");

	log("Adding a duplicate edge: should fail...",false);
	try {
		uuu_net.addEdge(vd1,vd0); // this is duplicate because the network is undirected
		throw FailedUnitTestException("Could insert a duplicate edge (not allowed)");
	}
	catch (DuplicateElementException& ex) {
		log("[FAIL] done!");
	}

	log("Adding an edge between non existing vertexes (should fail)...",false);
	try {
		uuu_net.addEdge(vd0,vd3+1);
		throw FailedUnitTestException("Non existing vertex not caught");
	}
	catch (ElementNotFoundException& ex) {
		log("[FAIL] done!");
	}

	log("Trying to add a weighed edge (should fail)...",false);
	try {
		uuu_net.addEdge(vd2,vd0,.45);
		throw FailedUnitTestException("Unsupported operation (add weighed edge) was allowed");
	}
	catch (OperationNotSupportedException& ex) {
		log("[FAIL] done!");
	}

	log("Trying to add a named edge (should fail)...",false);
	try {
		uuu_net.addEdge("a vertex name","another vertex name");
		//uuu_net.addEdge("a vertex name","another vertex name",.45);
		throw FailedUnitTestException("Unsupported operation (add named edge) was allowed");
	}
	catch (OperationNotSupportedException& ex) {
		log("[FAIL] done!");
	}

	log("Checking containsEdge function...",false);
	// the network is undirected, so (1-0) should be there
	if (!uuu_net.containsEdge(vd1,vd0)) throw FailedUnitTestException("Edge inserted, but containsEdge returns false");
	if (uuu_net.containsEdge(vd1,vd3)) throw FailedUnitTestException("Edge not present, but containsEdge returns true");
	log("done!");

	log("Checking neighborhood functions...",false);
	std::set<vertex_id> neigh_in = uuu_net.getInNeighbors(2);
	if (!neigh_in.count(vd0)==0) throw FailedUnitTestException("Vertex 0 is not 2s neighbor");
	if (!neigh_in.count(vd1)==1) throw FailedUnitTestException("Vertex 1 should be 2s neighbor");
	if (!neigh_in.count(vd2)==1) throw FailedUnitTestException("Vertex 2 should be 2s neighbor");
	std::set<vertex_id> neigh_out = uuu_net.getOutNeighbors(2);
	if (!neigh_out.count(vd0)==0) throw FailedUnitTestException("Vertex 0 is not 2s neighbor");
	if (!neigh_out.count(vd1)==1) throw FailedUnitTestException("Vertex 1 should be 2s neighbor");
	if (!neigh_out.count(vd2)==1) throw FailedUnitTestException("Vertex 2 should be 2s neighbor");
	log("done!");

	log("Adding attributes (v:color, v:height, e:name, e:redness) to node 0 and edge (1,2)...",false);
	uuu_net.newNumericVertexAttribute("height");
	uuu_net.newStringVertexAttribute("color");
	uuu_net.newNumericEdgeAttribute("redness");
	uuu_net.newStringEdgeAttribute("name");
	uuu_net.setNumericVertexAttribute(vd0,"height",35);
	uuu_net.setStringVertexAttribute(vd0,"color","blue");
	uuu_net.setNumericEdgeAttribute(vd1,vd2,"redness",255);
	uuu_net.setStringEdgeAttribute(vd1,vd2,"name","weak tie");
	log("done!");

	log("Retrieving sample values (should find everything, except redness between vd1 and vd3)...",false);
	if (uuu_net.getNumericVertexAttribute(vd0,"height")!=35 ||
		uuu_net.getStringVertexAttribute(vd0,"color")!="blue" ||
		uuu_net.getNumericEdgeAttribute(vd1,vd2,"redness")!=255 ||
		uuu_net.getStringEdgeAttribute(vd1,vd2,"name")!="weak tie")
		throw FailedUnitTestException("Previously set values could not be retrieved");
	else try {
		uuu_net.getNumericEdgeAttribute(vd1,vd3,"redness");
		throw FailedUnitTestException("No value is associated to edge (vd1,vd3), but no exception has been thrown");
	}
	catch (ElementNotFoundException& ex) {
		log("done!");
	}

	log("Removing a vertex (and as a cascade effect its adjacent edges) ...",false);
	uuu_net.deleteVertex(2);
	if (uuu_net.getNumVertexes()!=3) throw FailedUnitTestException("Wrong number of vertexes: " + std::to_string(uuu_net.getNumVertexes()));
	// two edges (1-2) and (2-2) must also have disappeared
	if (uuu_net.getNumEdges()!=2) throw FailedUnitTestException("Wrong number of edges: " + std::to_string(uuu_net.getNumEdges()));
	if (uuu_net.containsEdge(1,2)) throw FailedUnitTestException("Edge not present, but containsEdge returns true");
	log("done!");

	/**********************************************************************/
	log("**************************************************************");
	log("TESTING Network class (directed, weighed, named)");
	log("Creating the network...",false);
	Network dwn_net(true,true,true);
	log("done!");

	log("Adding four vertexes to the network...",false);
	vertex_id vid = dwn_net.addVertex("vd0");
	dwn_net.addVertex("vd1");
	dwn_net.addVertex("vd2");
	dwn_net.addVertex("vd3");
	if (dwn_net.getNumVertexes()!=4) throw FailedUnitTestException("Wrong number of vertexes: " + std::to_string(dwn_net.getNumVertexes()) + " instead of 4");
	if (vid!=0) throw FailedUnitTestException("Function not returning the id of the inserted vertex");
	log("done!");

	log("Trying to add an unnamed vertex (should fail)...",false);
	try {
		dwn_net.addVertex();
		throw FailedUnitTestException("Unsupported operation (add unnamed vertex) was allowed");
	}
	catch (OperationNotSupportedException& ex) {
		log("[FAIL] done!");
	}

	log("Trying to add a duplicate named vertex (should fail)...",false);
	try {
		dwn_net.addVertex("vd2");
		throw FailedUnitTestException("Unsupported operation (add duplicate named vertex) was allowed");
	}
	catch (DuplicateElementException& ex) {
		log("[FAIL] done!");
	}

	log("Adding four edges...",false);
	dwn_net.addEdge("vd0","vd1");
	dwn_net.addEdge("vd1","vd0");
	dwn_net.addEdge("vd1","vd2");
	dwn_net.addEdge("vd2","vd2");
	if (dwn_net.getNumEdges()!=4) throw FailedUnitTestException("Wrong number of edges");
	log("done!");

	log("Checking containsVertex function...",false);
	if (!dwn_net.containsVertex("vd3")) throw FailedUnitTestException("Vertex inserted, but containsVertex returns false");
	if (dwn_net.containsVertex("vd5")) throw FailedUnitTestException("Vertex not present, but containsVertex returns true");
	log("done!");

	log("Adding a duplicate edge (should fail)...",false);
	try {
		dwn_net.addEdge("vd0","vd1");
		throw FailedUnitTestException("Could insert a duplicate edge");
	}
	catch (DuplicateElementException& ex) {
		log("[FAIL] done!");
	}

	log("Adding an edge between non existing vertexes (should fail)...",false);
	try {
		dwn_net.addEdge("vd0","vd4");
		throw FailedUnitTestException("Non existing vertex not caught");
	}
	catch (ElementNotFoundException& ex) {
		log("[FAIL] done!");
	}

	log("Adding a weighed edge...",false);
	dwn_net.addEdge("vd2","vd0",.45);
	if (dwn_net.getEdgeWeight("vd2","vd0")!=.45) throw FailedUnitTestException("Incorrect weight found on edge (vd2,vd0): " + std::to_string(dwn_net.getEdgeWeight("vd2","vd0")));
	if (dwn_net.getEdgeWeight("vd2","vd2")!=1) throw FailedUnitTestException("Incorrect default weight found on edge (vd2,vd2): " + std::to_string(dwn_net.getEdgeWeight("vd2","vd2")));
	log("done!");

	log("Checking containsEdge function...",false);
	if (!dwn_net.containsEdge("vd1","vd0")) throw FailedUnitTestException("Edge inserted, but containsEdge returns false");
	if (dwn_net.containsEdge("vd1","vd3")) throw FailedUnitTestException("Edge not present, but containsEdge returns true");
	log("done!");

	log("Checking neighborhood functions...",false);
	std::set<std::string> sneigh_in = dwn_net.getInNeighbors("vd2");
	if (!sneigh_in.count("vd0")==0) throw FailedUnitTestException("Vertex vd0 is not vd2s neighbor");
	if (!sneigh_in.count("vd1")==1) throw FailedUnitTestException("Vertex vd1 should be vd2s neighbor");
	if (!sneigh_in.count("vd2")==1) throw FailedUnitTestException("Vertex vd2 should be vd2s neighbor");
	std::set<std::string> sneigh_out = dwn_net.getOutNeighbors("vd2");
	if (!sneigh_out.count("vd0")==1) throw FailedUnitTestException("Vertex vd0 should be vd2s neighbor");
	if (!sneigh_out.count("vd1")==0) throw FailedUnitTestException("Vertex vd1 is not vd2s neighbor");
	if (!sneigh_out.count("vd2")==1) throw FailedUnitTestException("Vertex vd2 is not vd2s neighbor");
	log("done!");

	log("Adding attributes (v:color, v:height, e:name, e:redness) to node 0 and edge (1,2)...",false);
	dwn_net.newNumericVertexAttribute("height");
	dwn_net.newStringVertexAttribute("color");
	dwn_net.newNumericEdgeAttribute("redness");
	dwn_net.newStringEdgeAttribute("name");
	dwn_net.setNumericVertexAttribute("vd0","height",35);
	dwn_net.setStringVertexAttribute("vd0","color","blue");
	dwn_net.setNumericEdgeAttribute("vd1","vd2","redness",255);
	dwn_net.setStringEdgeAttribute("vd1","vd2","name","weak tie");
	log("done!");

	log("Retrieving sample values (should find everything except redness)...",false);
	if (dwn_net.getNumericVertexAttribute("vd0","height")!=35 ||
		dwn_net.getStringVertexAttribute("vd0","color")!="blue" ||
		dwn_net.getNumericEdgeAttribute("vd1","vd2","redness")!=255 ||
		dwn_net.getStringEdgeAttribute("vd1","vd2","name")!="weak tie")
		throw FailedUnitTestException("Previously set values could not be retrieved");
	else try {
		dwn_net.getNumericEdgeAttribute("vd1","vd3","redness");
		throw FailedUnitTestException("No value is associated to edge (0,3), but no exception has been thrown");
	}
	catch (ElementNotFoundException& ex) {
		log("done!");
	}

	log("Removing a vertex (and as a cascade effect its adjacent edges) ...",false);
	dwn_net.deleteVertex("vd2");
	if (dwn_net.getNumVertexes()!=3) throw FailedUnitTestException("Wrong number of vertexes: " + std::to_string(dwn_net.getNumVertexes()));
	// edges adjacent to vd2 must also have disappeared
	if (dwn_net.getNumEdges()!=2) throw FailedUnitTestException("Wrong number of edges: " + std::to_string(dwn_net.getNumEdges()));
	if (dwn_net.containsEdge("vd1","vd2")) throw FailedUnitTestException("Edge not present, but containsEdge returns true");
	log("done!");

	std::set<vertex_id> vertexes = dwn_net.getVertexes();
	log("Iterating through vertexes:",false);
	for (vertex_id v: vertexes) {
		log(" " + dwn_net.getVertexName(v),false);
	}
	log(" done!");

	std::set<edge_id> edges = dwn_net.getEdges();
	log("Iterating through edges:",false);
	for (edge_id e: edges) {
		log(" " + e.to_string(),false);
	}
	log(" done!");

	log("TEST SUCCESSFULLY COMPLETED (Network class)");

	log("Printing the two tested networks:");
	print(uuu_net);
	print(dwn_net);
}


