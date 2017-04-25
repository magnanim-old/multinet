#include <Rcpp.h>
#include "multinet.h"
#include "r_functions.h"

using namespace Rcpp;
using namespace mlnet;

//namespace mlnet {
//class MLNetwork;
//}

//RCPP_EXPOSED_CLASS_NODECL(MLNetwork)
RCPP_EXPOSED_CLASS(RMLNetwork)
RCPP_EXPOSED_CLASS(REvolutionModel)
//RCPP_EXPOSED_CLASS(global_vertex_id)
//RCPP_EXPOSED_CLASS(global_edge_id)
//RCPP_EXPOSED_CLASS(edge_id)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Evolution
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*evolve(MultiplexNetwork &mnet,
       long num_of_steps,
       double pr_no_event[],
       double pr_internal_event[],
       std::vector<std::vector<double> > dependency,
       std::vector<EvolutionModel*> evolution_model) {

    double pr_no_event[],
    double pr_internal_event[],
    std::vector<std::vector<double> > dependency,
    std::vector<EvolutionModel*> evolution_model
}*/



//void show_global_vertex(global_vertex_id& gvid) {Rcout << "(vertex_id: " << gvid.vid << ", network_id: " << gvid.network << ")";}
//void show_network(Network* net) {Rcout << "network [" << (net->isNamed()?"named,":"unnamed,") << (net->isDirected()?"directed,":"undirected,") << (net->isWeighted()?"weigthed":"unweighted") << "] (vertexes: " << net->getNumVertexes() << ", edges: " << net->getNumEdges() << ")" << std::endl;}
void show_multilayer_network(RMLNetwork *mnet) {Rcout << mnet->get_mlnet()->to_string() << std::endl;}
void show_evolution_model(REvolutionModel *evm) {Rcout << evm->description() << std::endl;}

RCPP_MODULE(multinet){

    class_<RMLNetwork>("RMLNetwork")
	.method("name", &RMLNetwork::name, "name of the multilayer network" )
    .method("show", &show_multilayer_network);

    //class_<MLNetworkSharedPtr>("MLNetworkPtr");

    class_<REvolutionModel>("REvolutionModel")
    .method("show", &show_evolution_model);

    /*
    class_<EvolutionModel>("EvolutionModel");
    class_<BAEvolutionModel>("BAEvolutionModel")
    .derives<EvolutionModel>("EvolutionModel")
    .constructor<int,int>();
    class_<UniformEvolutionModel>("UniformEvolutionModel")
    .derives<EvolutionModel>("EvolutionModel")
    .constructor<int>();
     */

    /******************************/
    /* CREATION AND STORAGE       */
    /******************************/

    function("ml.empty", &emptyMultilayer, List::create( _["name"]="unnamed"), "Creates an empty multilayer network");

    // OTHER FUNCTIONS TO CREATE MLNETWORKS FROM THE LITERATURE (E.G., AUCS) ARE DEFINED IN functions.R

    // Generation
    function("evolution.pa.ml", &ba_evolution_model, List::create( _["m0"], _["m"]), "Creates a layer evolutionary model based on preferential attachment"); // TODO
    function("evolution.er.ml", &er_evolution_model, List::create( _["n"]), "Creates a layer evolutionary model based on random edge creation, as in the ER model"); // TODO
    function("grow.ml", &growMultiplex, List::create( _["num.actors"], _["num.steps"], _["models"], _["pr.internal"], _["pr.external"], _["dependency"]), "Grows a multiplex network"); // TODO

    // IO
    function("read.ml", &readMultilayer, List::create( _["file"], _["name"]="unnamed", _["sep"]=','), "Reads a multilayer network from a file");
    function("write.ml", &writeMultilayer, List::create( _["mlnetwork"], _["file"], _["format"]="multilayer", _["layers"]=CharacterVector(), _["sep"]=',', _["merge.actors"]=true, _["all.actors"]=false), "Writes a multilayer network to a file");

    /**************************************/
    /* INFORMATION ON MULTILAYER NETWORKS */
    /**************************************/

    function("layers.ml", &layers, List::create( _["mlnetwork"]), "Returns the list of layers in the input multilayer network");
    function("actors.ml", &actors, List::create( _["mlnetwork"], _["layers"]=CharacterVector()), "Returns the list of actors present in the input layers, or in the whole multilayer network if no layers are specified");
    function("nodes.ml", &nodes, List::create( _["mlnetwork"], _["layers"]=CharacterVector()), "Returns the list of nodes in the input layers, or in the whole multilayer network if no layers are specified");
    function("edges.ml", &edges, List::create( _["mlnetwork"], _["layers1"]=CharacterVector(), _["layers2"]=CharacterVector()), "Returns the list of edges among nodes in the input layers (if only one set of layers is specified), or from the first set of input layers to the second set of input layers, or in the whole multilayer network if no layers are specified");

    function("num.layers.ml", &numLayers, List::create( _["mlnetwork"]), "Returns the number of layers in the input mlnetwork");
    function("num.actors.ml", &numActors, List::create( _["mlnetwork"], _["layers"]=CharacterVector()), "Returns the number of actors in the set of input layers, or in the whole mlnetwork if no layers are specified");
    function("num.nodes.ml", &numNodes, List::create( _["mlnetwork"], _["layers"]=CharacterVector()), "Returns the number of nodes in the set of input layers, or in the whole mlnetwork if no layers are specified");
    function("num.edges.ml", &numEdges, List::create( _["mlnetwork"], _["layers1"]=CharacterVector(), _["layers2"]=CharacterVector()), "Returns the number of edges in the set of input layers, or in the whole mlnetwork if no layers are specified");

    function("is.directed.ml", &isDirected, List::create( _["mlnetwork"], _["layers1"]=CharacterVector(), _["layers2"]=CharacterVector()), "Returns a logical vector indicating for each pair of layers if it is directed or not");

    // NAVIGATION

    function("neighbors.ml", &actor_neighbors, List::create( _["mlnetwork"], _["actor"], _["layers"]=CharacterVector(), _["mode"] = "all"), "Returns the neighbors of a global identity on the set of input layers");
    function("xneighbors.ml", &actor_xneighbors, List::create( _["mlnetwork"], _["actor"], _["layers"]=CharacterVector(), _["mode"] = "all"), "Returns the exclusive neighbors of a global identity on the set of input layers");

    // NETWORK MANIPULATION

    function("add.layers.ml", &addLayers, List::create( _["mlnetwork"], _["layers"], _["directed"]=false), "Adds one or more layers to a multilayer network");
    function("add.actors.ml", &addActors, List::create( _["mlnetwork"], _["actors"]), "Adds one or more actors to a multilayer network");
    function("add.nodes.ml", &addNodes, List::create( _["mlnetwork"], _["nodes"]), "Adds one or more nodes to a layer of a multilayer network");
    function("add.edges.ml", &addEdges, List::create( _["mlnetwork"], _["edges"]), "Adds one or more edges to a multilayer network - each edge is a quadruple [actor,layer,actor,layer]");

    function("set.directed.ml", &setDirected, List::create( _["mlnetwork"], _["layers"], _["directed"]=true), "Set the directionality of one or more pairs of layers");

    function("delete.layers.ml", &deleteLayers, List::create( _["mlnetwork"], _["layers"]), "Deletes one or more layers from a multilayer network");
    function("delete.actors.ml", &deleteActors, List::create( _["mlnetwork"], _["actors"]), "Deletes one or more actors from a multilayer network");
    function("delete.nodes.ml", &deleteNodes, List::create( _["mlnetwork"], _["nodes"]), "Deletes one or more nodes from a layer of a multilayer network");
    function("delete.edges.ml", &deleteEdges, List::create( _["mlnetwork"], _["edges"]), "Deletes one or more edges from a multilayer network - each edge is a quadruple [actor,layer,actor,layer]");

    // ATTRIBUTE HANDLING

    function("new.attributes.ml", &newAttributes, List::create( _["mlnetwork"], _["attributes"], _["type"]="string", _["target"]="actor", _["layer"]="", _["layer1"]="", _["layer2"]=""), "Creates a new attribute so that values can be associated to actors, layers, nodes or edges");
    function("list.attributes.ml", &getAttributes, List::create( _["mlnetwork"], _["target"]="actor"), "Returns the list of attributes defined for the input multilayer network");
    function("get.values.ml", &getValues, List::create( _["mlnetwork"], _["attribute"], _["actors"]=CharacterVector(), _["layers"]=CharacterVector(), _["nodes"]=CharacterMatrix(0,0), _["edges"]=CharacterMatrix(0,0)), "Returns the value of an attribute on the specified actors, layers, nodes or edges");
    function("set.values.ml", &setValues, List::create( _["mlnetwork"], _["attribute"], _["actors"]=CharacterVector(), _["layers"]=CharacterVector(), _["nodes"]=CharacterMatrix(0,0), _["edges"]=CharacterMatrix(0,0), _["values"]), "Sets the value of an attribute for the specified actors/vertexes/edges");

    // TRANSFORMATION

    function("flatten.ml", &flatten, List::create( _["mlnetwork"], _["new.layer"]="flattening", _["layers"]=CharacterVector(), _["method"] = "weighted", _["force.directed"] = false, _["all.actors"] = false), "Adds a new layer with the actors in the input layers and an edge between A and B if they are connected in any of the merged layers");
    function("project.ml", &project, List::create( _["mlnetwork"], _["new.layer"]="projection", _["layer1"], _["layer2"], _["method"] = "clique"), "Adds a new layer with the actors in layer 1, and edges between actors A and B if they are connected to a common object in layer 2");

    // MEASURES

    function("degree.ml", &degree_ml, List::create( _["mlnetwork"], _["actors"]=CharacterVector(), _["layers"]=CharacterVector(), _["mode"] = "all"), "Returns the degree of each actor");
    function("degree.deviation.ml", &degree_deviation_ml, List::create( _["mlnetwork"], _["actors"]=CharacterVector(), _["layers"]=CharacterVector(), _["mode"] = "all"), "Returns the standard deviation of the degree of each actor on the specified layers");
    function("occupation.ml", &occupation_ml, List::create( _["mlnetwork"], _["transitions"], _["teleportation"]=.2, _["steps"]=0), "Returns the occupation centrality value of each actor");
    function("neighborhood.ml", &neighborhood_ml, List::create( _["mlnetwork"], _["actors"]=CharacterVector(), _["layers"]=CharacterVector(), _["mode"] = "all"), "Returns the neighborhood of each actor");
    function("xneighborhood.ml", &xneighborhood_ml, List::create( _["mlnetwork"], _["actors"]=CharacterVector(), _["layers"]=CharacterVector(), _["mode"] = "all"), "Returns the exclusive neighborhood of each actor");
    function("connective.redundancy.ml", &connective_redundancy_ml, List::create( _["mlnetwork"], _["actors"]=CharacterVector(), _["layers"]=CharacterVector(), _["mode"] = "all"), "Returns the connective redundancy of each actor");
    function("relevance.ml", &relevance_ml, List::create( _["mlnetwork"], _["actors"]=CharacterVector(), _["layers"]=CharacterVector(), _["mode"] = "all"), "Returns the layer relevance of each actor");
    function("xrelevance.ml", &xrelevance_ml, List::create( _["mlnetwork"], _["actors"]=CharacterVector(), _["layers"]=CharacterVector(), _["mode"] = "all"), "Returns the exclusive layer relevance of each actor");
    function("similarity.ml", &similarity_ml, List::create( _["mlnetwork"], _["layer1"], _["layer2"], _["method"] = "jaccard.edges"), "Computes the similarity between the input layers");
    function("correlation.ml", &correlation_ml, List::create( _["mlnetwork"], _["layer1"], _["layer2"], _["method"] = "pearson.degree", _["mode"] = "all"), "Computes the correlation between the input layers");
    function("distance.ml", &distance_ml, List::create( _["mlnetwork"], _["from"], _["to"]=CharacterVector(), _["method"] = "multiplex"), "Computes the distance between two actors");

    // CLUSTERING
    //function("clique.percolation.ml", &cliquepercolation_ml, List::create( _["mlnetwork"], _["k"]=3, _["m"]=1), "Extension of the clique percolation method");


    // FOR VISUALIZATION
    function("layout.multiforce.ml", &multiforce_ml, List::create( _["mlnetwork"], _["layers"]=CharacterVector(), _["w_in"], _["w_inter"], _["iterations"]=100), "Multiforce method: computes node coordinates");
    
    //function("sir.ml", &sir_ml, List::create( _["mlnetwork"], _["beta"], _["tau"], _["num_iterations"] = 1000), "Executes a SIR spreading process, returning the number of nodes in each status at each iteration");
}


