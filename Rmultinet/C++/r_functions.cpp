/*
 * r_functions.cpp
 *
 * Created on: Jun 19, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "r_functions.h"
#include "rcpp_utils.h"

using namespace mlnet;
using namespace Rcpp;

RCPP_EXPOSED_CLASS(RMLNetwork)
RCPP_EXPOSED_CLASS(REvolutionModel)

//RCPP_EXPOSED_CLASS(MLNetwork)
//RCPP_EXPOSED_CLASS(REvolutionModel)
//RCPP_EXPOSED_CLASS(BAEvolutionModel)

// CREATION AND STORAGE

RMLNetwork emptyMultilayer(const std::string& name) {
	return RMLNetwork(MLNetwork::create(name));
}

void renameMultilayer(RMLNetwork& rmnet, const std::string& new_name) {
	rmnet.get_mlnet()->name = new_name;
}

RMLNetwork readMultilayer(const std::string& input_file, const std::string& name, char sep) {
	return RMLNetwork(read_multilayer(input_file,name,sep));
}

void writeMultilayer(const RMLNetwork& rmnet, const std::string& output_file, const std::string& format, const CharacterVector& layer_names, char sep, bool merge_actors, bool all_actors) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);

	if (format=="multilayer")
		write_multilayer(mnet,output_file,sep);
	else if (format=="graphml") {
		if (!merge_actors && all_actors) {
			Rcout << "option all.actors not used when merge.actors=FALSE" << std::endl;
		}
		write_graphml(mnet,output_file,layers,merge_actors,all_actors);
	}
	else stop("unexpected value: format " + format);
}

REvolutionModel ba_evolution_model(int m0, int m) {
	EvolutionModelSharedPtr ba(new BAEvolutionModel(m0,m));
	return REvolutionModel(ba,"Preferential attachment evolution model (" + to_string(m0) + "," + to_string(m) + ")");
}

REvolutionModel er_evolution_model(int n) {
	EvolutionModelSharedPtr er(new UniformEvolutionModel(n));
	return REvolutionModel(er,"Uniform evolution model (" + to_string(n) + ")");
}


RMLNetwork growMultiplex(int num_actors, long num_of_steps, const GenericVector& evolution_model, const NumericVector& pr_internal_event, const NumericVector& pr_external_event, const NumericMatrix& dependency) {
	if (num_actors<=0) stop("The number of actors must be positive");
	if (num_actors<=0) stop("The number of steps must be positive");
	int num_layers = evolution_model.size();
	if (dependency.nrow()!=num_layers || dependency.ncol()!=num_layers ||
			pr_internal_event.size()!=num_layers || pr_external_event.size()!=num_layers)
		stop("The number of evolution models, evolution probabilities and the number of rows/columns of the dependency matrix must be the same");
	MLNetworkSharedPtr mnet = MLNetwork::create("synthetic");
	for (int i=0; i<num_layers; i++) {
		mnet->add_layer("L"+to_string(i),UNDIRECTED);
	}
	std::vector<double> pr_int(pr_internal_event.size());
	for (int i=0; i<pr_internal_event.size(); i++)
		pr_int[i] = pr_internal_event.at(i);
	std::vector<double> pr_ext(pr_external_event.size());
	for (int i=0; i<pr_external_event.size(); i++)
		pr_ext[i] = pr_external_event.at(i);
	std::vector<std::vector<double> > dep(dependency.nrow());
	for (int i=0; i<dependency.nrow(); i++) {
		std::vector<double> row(dependency.ncol());
		for (int j=0; j<dependency.ncol(); j++) {
			row[j] = dependency(i,j);
		}
		dep[i] = row;
	}
	std::vector<EvolutionModelSharedPtr> models(evolution_model.size());
	for (int i=0; i<models.size(); i++) {
		models[i] = (as<REvolutionModel>(evolution_model[i])).get_model();
	}
	evolve(mnet,num_of_steps,num_actors,pr_int,pr_ext,dep,models);
	return RMLNetwork(mnet);
}

// INFORMATION ON NETWORKS

CharacterVector layers(const RMLNetwork& rmnet) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    CharacterVector res(0);
    for (LayerSharedPtr layer: *mnet->get_layers()) {
        res.push_back(layer->name);
    }
    return res;
}

CharacterVector actors(const RMLNetwork& rmnet, const CharacterVector& layer_names) {
	CharacterVector actors(0);
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    if (layer_names.size()==0) {
    	for (ActorSharedPtr actor: *mnet->get_actors())
            actors.push_back(actor->name);
    }
    else {
    	std::vector<LayerSharedPtr> layers = resolve_layers(mnet,layer_names);
    	for (LayerSharedPtr layer: layers) {
  	    	for (NodeSharedPtr node: *mnet->get_nodes(layer)) {
    			actors.push_back(node->actor->name);
    		}
    	}
    }
	//for (ActorSharedPtr actor: actors)
      //  res.push_back(actor->name);
    return actors;
}

CharacterMatrix nodes(const RMLNetwork& rmnet, const CharacterVector& layer_names) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	std::vector<LayerSharedPtr> layers = resolve_layers(mnet,layer_names);
	long num_nodes = 0;
	for (LayerSharedPtr layer: layers) {
		num_nodes += mnet->get_nodes(layer)->size();
	}
	CharacterMatrix res(num_nodes,2);
	CharacterVector rownames(0);
	CharacterVector colnames = CharacterVector::create("Actor","Layer");
	res.attr("dimnames") = List::create(rownames, colnames);
	int j=0;
	for (LayerSharedPtr layer: layers) {
		for (NodeSharedPtr node: *mnet->get_nodes(layer)) {
			res(j,0)=node->actor->name;
			res(j,1)=layer->name;
			j++;
		}
	}
    return res;
}

DataFrame edges_idx(const RMLNetwork& rmnet) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    NumericVector from, to, directed;
    NodeListSharedPtr nodes = mnet->get_nodes();
    EdgeListSharedPtr edges = mnet->get_edges();
    for (EdgeSharedPtr edge: *edges) {
        from.push_back(nodes->get_index(edge->v1));
        to.push_back(nodes->get_index(edge->v2));
        directed=(edge->directionality==DIRECTED)?1:0;
    }
    return DataFrame::create(_["from"] = from, _["to"] = to, _["dir"] = directed );
}

CharacterMatrix edges(const RMLNetwork& rmnet, const CharacterVector& layer_names1, const CharacterVector& layer_names2) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
		std::vector<LayerSharedPtr> layers1 = resolve_layers(mnet,layer_names1);
		std::vector<LayerSharedPtr> layers2;
		if (layer_names2.size()==0)
			layers2 = layers1;
		else layers2 = resolve_layers(mnet,layer_names2);
		long num_edges = 0;
		for (LayerSharedPtr layer1: layers1) {
			for (LayerSharedPtr layer2: layers2) {
				if (layer1>layer2 && !mnet->is_directed(layer1,layer2))
					continue;
				num_edges += mnet->get_edges(layer1,layer2)->size();
			}
		}
		CharacterMatrix res(num_edges,5);
		CharacterVector rownames(0);
		CharacterVector colnames = CharacterVector::create("ActorFrom","LayerFrom","ActorTo","LayerTo","Directed");
		res.attr("dimnames") = List::create(rownames, colnames);
		int j=0;
		for (LayerSharedPtr layer1: layers1) {
			for (LayerSharedPtr layer2: layers2) {
				if (layer1>layer2 && !mnet->is_directed(layer1,layer2))
					continue;
				for (EdgeSharedPtr edge: *mnet->get_edges(layer1,layer2)) {
					res(j,0)=edge->v1->actor->name;
					res(j,1)=layer1->name;
					res(j,2)=edge->v2->actor->name;
					res(j,3)=layer2->name;
					res(j,4)=(edge->directionality==DIRECTED)?true:false;
					j++;
				}
			}
		}
	    return res;
	}

int numLayers(const RMLNetwork& rmnet) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	return mnet->get_layers()->size();
}

long numActors(const RMLNetwork& rmnet, const CharacterVector& layer_names) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	if (layer_names.size()==0)
		return mnet->get_actors()->size();
	std::vector<LayerSharedPtr> layers = resolve_layers(mnet,layer_names);
    std::unordered_set<actor_id> actors;
	for (LayerSharedPtr layer: layers) {
		for (NodeSharedPtr node: *mnet->get_nodes(layer)) {
			actors.insert(node->actor->id);
		}
	}
	return actors.size();
}

long numNodes(const RMLNetwork& rmnet, const CharacterVector& layer_names) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	std::vector<LayerSharedPtr> layers = resolve_layers(mnet,layer_names);
	long num_nodes = 0;
	for (LayerSharedPtr layer: layers) {
		num_nodes += mnet->get_nodes(layer)->size();
	}
	return num_nodes;
}

long numEdges(const RMLNetwork& rmnet, const CharacterVector& layer_names1, const CharacterVector& layer_names2) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	std::vector<LayerSharedPtr> layers1 = resolve_layers(mnet,layer_names1);
	std::vector<LayerSharedPtr> layers2;
	if (layer_names2.size()==0)
		layers2 = layers1;
	else layers2 = resolve_layers(mnet,layer_names2);
	long num_edges = 0;
	for (LayerSharedPtr layer1: layers1) {
		for (LayerSharedPtr layer2: layers2) {
			if (layer1<layer2 && !mnet->is_directed(layer1,layer2))
				continue;
			num_edges += mnet->get_edges(layer1,layer2)->size();
		}
	}
	return num_edges;
}

CharacterMatrix isDirected(const RMLNetwork& rmnet, const CharacterVector& layer_names1, const CharacterVector& layer_names2) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	std::vector<LayerSharedPtr> layers1 = resolve_layers(mnet,layer_names1);
	std::vector<LayerSharedPtr> layers2;
	if (layer_names2.size()==0)
		layers2 = layers1;
	else layers2 = resolve_layers(mnet,layer_names2);

	CharacterMatrix res(layers1.size()*layers2.size(),3);
	CharacterVector rownames(0);
	CharacterVector colnames = CharacterVector::create("Layer1","Layer2","Directed");
	res.attr("dimnames") = List::create(rownames, colnames);
	int j=0;
	for (LayerSharedPtr layer1: layers1) {
		for (LayerSharedPtr layer2: layers2) {
			res(j,0)=layer1->name;
			res(j,1)=layer2->name;
			res(j,2)=mnet->is_directed(layer1,layer2);
			j++;
		}
	}
    return res;
}

std::unordered_set<std::string> actor_neighbors(const RMLNetwork& rmnet, const std::string& actor_name, const CharacterVector& layer_names, const std::string& mode_name) {
    std::unordered_set<std::string> res_neighbors;
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	ActorSharedPtr actor = mnet->get_actor(actor_name);
	if (!actor) {
		stop("actor " + actor_name + " not found");
	}
	std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);
	edge_mode mode = INOUT;
	if (mode_name == "all")
		mode = INOUT;
	else if (mode_name == "in")
		mode = IN;
	else if (mode_name == "out")
		mode = OUT;
	else
        stop("unexpected value: mode " + mode_name);
    ActorListSharedPtr actors = neighbors(mnet,actor,layers,mode);
    for (ActorSharedPtr neigh: *actors) {
        res_neighbors.insert(neigh->name);
    }
	return res_neighbors;
}

std::unordered_set<std::string> actor_xneighbors(const RMLNetwork& rmnet, const std::string& actor_name, const CharacterVector& layer_names, const std::string& mode_name) {
	std::unordered_set<std::string> res_xneighbors;
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	ActorSharedPtr actor = mnet->get_actor(actor_name);
	if (!actor) {
		stop("actor " + actor_name + " not found");
	}
	std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);
	edge_mode mode = INOUT;
	if (mode_name == "all")
		mode = INOUT;
	else if (mode_name == "in")
		mode = IN;
	else if (mode_name == "out")
		mode = OUT;
	else
        stop("unexpected value: mode " + mode_name);
    ActorListSharedPtr actors = xneighbors(mnet,actor,layers,mode);
    for (ActorSharedPtr neigh: *actors) {
		res_xneighbors.insert(neigh->name);
	}
	return res_xneighbors;
}


// NETWORK MANIPULATION

void addLayers(RMLNetwork& rmnet, const CharacterVector& layer_names, const LogicalVector& directed) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	if (directed.size()==1) {
		for (int i=0; i<layer_names.size(); i++) {
			mnet->add_layer(std::string(layer_names[i]),directed[0]?DIRECTED:UNDIRECTED);
		}
	}
	else if (layer_names.size()!=directed.size())
		stop("Same number of layer names and layer directionalities expected");
	else {
		for (int i=0; i<layer_names.size(); i++) {
				mnet->add_layer(std::string(layer_names[i]),directed[i]?DIRECTED:UNDIRECTED);
		}
	}
}

void addActors(RMLNetwork& rmnet, const CharacterVector& actor_names) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	for (int i=0; i<actor_names.size(); i++) {
		mnet->add_actor(std::string(actor_names[i]));
	}
}

void addNodes(RMLNetwork& rmnet, const CharacterVector& nodes) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	if (nodes.size()%2 != 0)
		stop("Nodes must be specified as actor/layer names (i.e., two entries for each node)");
	int num_nodes = nodes.size()/2;
	for (int i=0; i<num_nodes; i++) {
		ActorSharedPtr actor = mnet->get_actor(std::string(nodes(i*2)));
		if (!actor) stop("cannot find actor " + std::string(nodes(i*2)));
		LayerSharedPtr layer = mnet->get_layer(std::string(nodes(i*2+1)));
		if (!layer) stop("cannot find layer " + std::string(nodes(i*2+1)));
		mnet->add_node(actor,layer);
	}
}

void addEdges(RMLNetwork& rmnet, const CharacterVector& edges) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	if (edges.size()%4 != 0)
		stop("Edges must be specified as pairs of actor/layer names (i.e., four entries for each edge)");
	int num_edges = edges.size()/4;
	for (int i=0; i<num_edges; i++) {
		ActorSharedPtr actor1 = mnet->get_actor(std::string(edges(i*4)));
		if (!actor1) stop("cannot find actor " + std::string(edges(i*4)));
		ActorSharedPtr actor2 = mnet->get_actor(std::string(edges(i*4+2)));
		if (!actor2) stop("cannot find actor " + std::string(edges(i*4+2)));
		LayerSharedPtr layer1 = mnet->get_layer(std::string(edges(i*4+1)));
		if (!layer1) stop("cannot find layer " + std::string(edges(i*4+1)));
		LayerSharedPtr layer2 = mnet->get_layer(std::string(edges(i*4+3)));
		if (!layer2) stop("cannot find layer " + std::string(edges(i*4+3)));
		NodeSharedPtr node1 = mnet->get_node(actor1,layer1);
		if (!node1) stop("cannot find node " + actor1->name + " " + layer1->name);
		NodeSharedPtr node2 = mnet->get_node(actor2,layer2);
		if (!node2) stop("cannot find node " + actor2->name + " " + layer2->name);
		mnet->add_edge(node1,node2);
	}
}

void setDirected(const RMLNetwork& rmnet, const CharacterVector& layers, bool directed) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	if (layers.size()%2 != 0)
		stop("The layers to be affected must be specified as pairs of layers names (i.e., two entries for each edge directionality)");
	int num_layers = layers.size()/2;

	for (int i=0; i<num_layers; i++) {
		LayerSharedPtr layer1 = mnet->get_layer(std::string(layers(i*2)));
		if (!layer1) stop("cannot find layer " + std::string(layers(i*2)));
		LayerSharedPtr layer2 = mnet->get_layer(std::string(layers(i*2+1)));
		if (!layer2) stop("cannot find layer " + std::string(layers(i*2+1)));
		mnet->set_directed(layer1,layer2,directed);
	}
}

void deleteLayers(RMLNetwork& rmnet, const CharacterVector& layer_names) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	for (int i=0; i<layer_names.size(); i++) {
		LayerSharedPtr layer = mnet->get_layer(std::string(layer_names(i)));
		mnet->erase(layer);
	}
}

void deleteActors(RMLNetwork& rmnet, const CharacterVector& actor_names) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	for (int i=0; i<actor_names.size(); i++) {
		ActorSharedPtr actor = mnet->get_actor(std::string(actor_names(i)));
		mnet->erase(actor);
	}
}

void deleteNodes(RMLNetwork& rmnet, const CharacterVector& node_matrix) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	std::vector<NodeSharedPtr> nodes = resolve_nodes(mnet,node_matrix);

	for (NodeSharedPtr node: nodes) {
		mnet->erase(node);
	}
}

void deleteEdges(RMLNetwork& rmnet, const CharacterVector& edge_matrix) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	std::vector<EdgeSharedPtr> edges = resolve_edges(mnet, edge_matrix);
	for (EdgeSharedPtr edge: edges) {
		mnet->erase(edge);
	}
}

void newAttributes(RMLNetwork& rmnet, const CharacterVector& attribute_names, const std::string& type, const std::string& target, const std::string& layer_name, const std::string& layer_name1, const std::string& layer_name2) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	attribute_type a_type;
	if (type=="string") {
		a_type = STRING_TYPE;
	}
	else if (type=="numeric") {
		a_type = NUMERIC_TYPE;
	}
	else stop("Wrong type");
	if (target=="actor") {
		if (layer_name!="" || layer_name1!="" || layer_name2!="")
			stop("No layers should be specified for target 'actor'");
		for (int i=0; i<attribute_names.size(); i++) {
				mnet->actor_features()->add(std::string(attribute_names[i]),a_type);
		}
	}
	else if (target=="layer") {
		if (layer_name!="" || layer_name1!="" || layer_name2!="")
			stop("No layers should be specified for target 'layer'");
		for (int i=0; i<attribute_names.size(); i++) {
			mnet->layer_features()->add(std::string(attribute_names[i]),a_type);
		}
	}
	else if (target=="node") {
		if (layer_name1!="" || layer_name2!="")
			stop("layer1 and layer2 should not be specified for target 'node'");
		LayerSharedPtr layer = mnet->get_layer(layer_name);
		if (!layer) stop("layer " + layer_name + " not found");
		for (int i=0; i<attribute_names.size(); i++) {
			mnet->node_features(layer)->add(std::string(attribute_names[i]),a_type);
		}
	}
	else if (target=="edge") {
		if (layer_name!="" && (layer_name1!="" || layer_name2!=""))
			stop("either layers (for intra-layer edges) or layers1 and layers2 (for inter-layer edges) must be specified for target 'edge'");
		LayerSharedPtr layer1, layer2;

		if (layer_name1=="") {
			layer1 = mnet->get_layer(layer_name);
			layer2 = layer1;
			if (!layer1) stop("layer " + layer_name + " not found");
		}
		else if (layer_name2!="") {
			layer1 = mnet->get_layer(layer_name1);
			layer2 = mnet->get_layer(layer_name2);
		}
		else stop("if layer1 is specified, also layer2 is required");
		for (int i=0; i<attribute_names.size(); i++) {
			mnet->edge_features(layer1,layer2)->add(std::string(attribute_names[i]),a_type);
		}
	}
	else stop("wrong target: " + target);
}

GenericMatrix getAttributes(const RMLNetwork& rmnet, const std::string& target) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	if (target=="actor") {
		AttributeStoreSharedPtr store = mnet->actor_features();
		int num_attrs = store->numAttributes();
		GenericMatrix res(num_attrs,2);
		CharacterVector rownames(0);
		CharacterVector colnames = CharacterVector::create("name","type");
		res.attr("dimnames") = List::create(rownames, colnames);
		for (int j=0; j<num_attrs; j++) {
			AttributeSharedPtr att=store->attribute(j);
			res(j,0) = att->name();
			res(j,1) = att->type_as_string();
		}
		return res;
	}
	else if (target=="layer") {
		AttributeStoreSharedPtr store = mnet->layer_features();
		int num_attrs = store->numAttributes();
		GenericMatrix res(num_attrs,2);
		CharacterVector rownames(0);
		CharacterVector colnames = CharacterVector::create("name","type");
		res.attr("dimnames") = List::create(rownames, colnames);
		for (int j=0; j<num_attrs; j++) {
			AttributeSharedPtr att=store->attribute(j);
			res(j,0) = att->name();
			res(j,1) = att->type_as_string();
		}
		return res;
	}
	else if (target=="node") {
		int num_attrs = 0;
		for (LayerSharedPtr layer: *mnet->get_layers())
			num_attrs += mnet->node_features(layer)->numAttributes();
		GenericMatrix res(num_attrs,3);
		CharacterVector rownames(0);
		CharacterVector colnames = CharacterVector::create("name","type","layer");
		res.attr("dimnames") = List::create(rownames, colnames);
		int j=0;
		for (LayerSharedPtr layer: *mnet->get_layers()) {
			AttributeStoreSharedPtr store=mnet->node_features(layer);
			for (int i=0; i<store->numAttributes(); i++) {
				AttributeSharedPtr att = store->attribute(i);
				res(j,0) = att->name();
				res(j,1) = att->type_as_string();
				res(j,2) = layer->name;
				j++;
			}
		}
		return res;
	}
	else if (target=="edge") {
		int num_attrs = 0;
		for (LayerSharedPtr layer1: *mnet->get_layers()) {
			for (LayerSharedPtr layer2: *mnet->get_layers()) {
				if (!mnet->is_directed(layer1,layer2) && layer1->name>layer2->name) continue;
				num_attrs += mnet->edge_features(layer1,layer2)->numAttributes();
			}
		}
		GenericMatrix res(num_attrs,4);
		CharacterVector rownames(0);
		CharacterVector colnames = CharacterVector::create("name","type","layer1","layer2");
		res.attr("dimnames") = List::create(rownames, colnames);
		int j=0;
		for (LayerSharedPtr layer1: *mnet->get_layers()) {
			for (LayerSharedPtr layer2: *mnet->get_layers()) {
				if (!mnet->is_directed(layer1,layer2) && layer1->name>layer2->name) continue;
				AttributeStoreSharedPtr store=mnet->edge_features(layer1,layer2);
				for (int i=0; i<store->numAttributes(); i++) {
					AttributeSharedPtr att = store->attribute(i);
					res(j,0) = att->name();
					res(j,1) = att->type_as_string();
					res(j,2) = layer1->name;
					res(j,3) = layer2->name;
					j++;
				}
			}
		}
		return res;
	}
	else stop("wrong target: " + target);
	return 0; // never gets here
}

GenericMatrix getValues(RMLNetwork& rmnet, const std::string& attribute_name, const CharacterVector& actor_names, const CharacterVector& layer_names, const CharacterVector& node_matrix, const CharacterVector& edge_matrix) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	if (actor_names.size() != 0) {
		if (layer_names.size() > 0)
			Rcout << "Warning: unused parameter: \"layers\"" << std::endl;
		if (node_matrix.size() > 0)
			Rcout << "Warning: unused parameter: \"nodes\"" << std::endl;
		if (edge_matrix.size() > 0)
			Rcout << "Warning: unused parameter: \"edges\"" << std::endl;
		std::vector<ActorSharedPtr> actors = resolve_actors(mnet,actor_names);
		GenericMatrix res(actors.size(),2);
		CharacterVector rownames(0);
		CharacterVector colnames = CharacterVector::create("actor","value");
		res.attr("dimnames") = List::create(rownames, colnames);
		int j=0;

		AttributeStoreSharedPtr store = mnet->actor_features();
		AttributeSharedPtr att = store->attribute(attribute_name);
		if (!att) {
			stop("cannot find attribute: " + attribute_name + " for actors");
		}
		for (ActorSharedPtr actor: actors) {
			res(j,0) = actor->name;
			switch (att->type()) {
			case NUMERIC_TYPE:
				res(j,1) = store->getNumeric(actor->id,att->name());
				break;
			case STRING_TYPE:
				res(j,1) = store->getString(actor->id,att->name());
				break;
			}
			j++;
		}
		return res;
	}
	else if (layer_names.size() != 0) {
		if (node_matrix.size() > 0)
			Rcout << "Warning: unused parameter: \"nodes\"" << std::endl;
		if (edge_matrix.size() > 0)
			Rcout << "Warning: unused parameter: \"edges\"" << std::endl;
		std::vector<LayerSharedPtr> layers = resolve_layers(mnet,layer_names);
		GenericMatrix res(layers.size(),2);
		CharacterVector rownames(0);
		CharacterVector colnames = CharacterVector::create("layer","value");
		res.attr("dimnames") = List::create(rownames, colnames);
		int j=0;

		AttributeStoreSharedPtr store = mnet->layer_features();
		AttributeSharedPtr att = store->attribute(attribute_name);
		if (!att) {
			stop("cannot find attribute: " + attribute_name + " for layers");
		}
		for (LayerSharedPtr layer: layers) {
			res(j,0) = layer->name;
			switch (att->type()) {
			case NUMERIC_TYPE:
				res(j,1) = store->getNumeric(layer->id,att->name());
				break;
			case STRING_TYPE:
				res(j,1) = store->getString(layer->id,att->name());
				break;
			}
			j++;
		}
		return res;
	}
	// local attributes: vertexes
	else if (node_matrix.size() > 0) {
		if (edge_matrix.size() > 0)
			Rcout << "Warning: unused parameter: \"edges\"" << std::endl;
		std::vector<NodeSharedPtr> nodes = resolve_nodes(mnet,node_matrix);
		GenericMatrix res(nodes.size(),3);
		CharacterVector rownames(0);
		CharacterVector colnames = CharacterVector::create("actor","layer","value");
		res.attr("dimnames") = List::create(rownames, colnames);
		int j=0;

		for (NodeSharedPtr node: nodes) {
			AttributeStoreSharedPtr store = mnet->node_features(node->layer);
			AttributeSharedPtr att = store->attribute(attribute_name);
			if (!att) {
				stop("cannot find attribute: " + attribute_name + " for nodes on layer " + node->layer->name);
			}
			res(j,0) = node->actor->name;
			res(j,1) = node->layer->name;
			switch (att->type()) {
			case NUMERIC_TYPE:
				res(j,2) = store->getNumeric(node->id,att->name());
				break;
			case STRING_TYPE:
				res(j,2) = store->getString(node->id,att->name());
				break;
			}
			j++;
		}
		return res;
	}
	else if (edge_matrix.size() > 0) {
		std::vector<EdgeSharedPtr> edges = resolve_edges(mnet,edge_matrix);
		GenericMatrix res(edges.size(),5);
		CharacterVector rownames(0);
		CharacterVector colnames = CharacterVector::create("actor1","layer1","actor2","layer2","value");
		res.attr("dimnames") = List::create(rownames, colnames);
		int j=0;

		for (EdgeSharedPtr edge: edges) {
			AttributeStoreSharedPtr store = mnet->edge_features(edge->v1->layer,edge->v2->layer);
			AttributeSharedPtr att = store->attribute(attribute_name);
			if (!att) {
				stop("cannot find attribute: " + attribute_name + " for edges on layers " + edge->v1->layer->name + ", " + edge->v2->layer->name);
			}
			res(j,0) = edge->v1->actor->name;
			res(j,1) = edge->v1->layer->name;
			res(j,2) = edge->v2->actor->name;
			res(j,3) = edge->v2->layer->name;
			switch (att->type()) {
			case NUMERIC_TYPE:
				res(j,4) = store->getNumeric(edge->id,att->name());
				break;
			case STRING_TYPE:
				res(j,4) = store->getString(edge->id,att->name());
				break;
			}
			j++;
		}
		return res;
	}
	else {
		stop("Required at least one parameter: \"actors\", \"layers\", \"nodes\" or \"edges\"");
	}
	// Never gets here
	return 0;
}

void setValues(RMLNetwork& rmnet, const std::string& attribute_name, const CharacterVector& actor_names, const CharacterVector& layer_names, const CharacterVector& node_matrix, const CharacterVector& edge_matrix, const GenericVector& values) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	if (actor_names.size() != 0) {
		if (actor_names.size() != values.size() && values.size()!=1)
			stop("wrong number of values");
		if (layer_names.size() > 0)
			Rcout << "Warning: unused parameter: \"layers\"" << std::endl;
		if (node_matrix.size() > 0)
			Rcout << "Warning: unused parameter: \"nodes\"" << std::endl;
		if (edge_matrix.size() > 0)
			Rcout << "Warning: unused parameter: \"edges\"" << std::endl;
		std::vector<ActorSharedPtr> actors = resolve_actors(mnet,actor_names);
		AttributeStoreSharedPtr store = mnet->actor_features();
		AttributeSharedPtr att = store->attribute(attribute_name);
		if (!att) {
			stop("cannot find attribute: " + attribute_name + " for actors");
		}
		int i=0;
		for (ActorSharedPtr actor: actors) {
			switch (att->type()) {
			case NUMERIC_TYPE:
				if (values.size()==1)
					store->setNumeric(actor->id,att->name(),as<double>(values[0]));
				else
					store->setNumeric(actor->id,att->name(),as<double>(values[i]));
				break;
			case STRING_TYPE:
				if (values.size()==1)
					store->setString(actor->id,att->name(),as<std::string>(values[0]));
				else
					store->setString(actor->id,att->name(),as<std::string>(values[i]));
				break;
			}
			i++;
		}
	}
	else if (layer_names.size() != 0) {
		if (layer_names.size() != values.size() && values.size()!=1)
			stop("wrong number of values");
		if (node_matrix.size() > 0)
			Rcout << "Warning: unused parameter: \"nodes\"" << std::endl;
		if (edge_matrix.size() > 0)
			Rcout << "Warning: unused parameter: \"edges\"" << std::endl;
		std::vector<LayerSharedPtr> layers = resolve_layers(mnet,layer_names);
		AttributeStoreSharedPtr store = mnet->layer_features();
		AttributeSharedPtr att = store->attribute(attribute_name);
		if (!att) {
			stop("cannot find attribute: " + attribute_name + " for layers");
		}
		int i=0;
		for (LayerSharedPtr layer: layers) {
			switch (att->type()) {
			case NUMERIC_TYPE:
				if (values.size()==1)
					store->setNumeric(layer->id,att->name(),as<double>(values[0]));
				else
					store->setNumeric(layer->id,att->name(),as<double>(values[i]));
				break;
			case STRING_TYPE:
				if (values.size()==1)
					store->setString(layer->id,att->name(),as<std::string>(values[0]));
				else
					store->setString(layer->id,att->name(),as<std::string>(values[i]));
				break;
			}
			i++;
		}
	}
	// local attributes: nodes
	else if (node_matrix.size() > 0) {
		if (edge_matrix.size() > 0)
			Rcout << "Warning: unused parameter: \"edges\"" << std::endl;
		std::vector<NodeSharedPtr> nodes = resolve_nodes(mnet,node_matrix);
		if (nodes.size() != values.size() && values.size()!=1)
			stop("wrong number of values");
		int i=0;
		for (NodeSharedPtr node: nodes) {
			AttributeStoreSharedPtr store = mnet->node_features(node->layer);
			AttributeSharedPtr att = store->attribute(attribute_name);
			if (!att) {
				stop("cannot find attribute: " + attribute_name + " for nodes on layer " + node->layer->name);
			}
			switch (att->type()) {
			case NUMERIC_TYPE:
				if (values.size()==1)
					store->setNumeric(node->id,att->name(),as<double>(values[0]));
				else
					store->setNumeric(node->id,att->name(),as<double>(values[i]));
				break;
			case STRING_TYPE:
				if (values.size()==1)
					store->setString(node->id,att->name(),as<std::string>(values[0]));
				else
					store->setString(node->id,att->name(),as<std::string>(values[i]));
				break;
			}
			i++;
		}
	}
	else if (edge_matrix.size() > 0) {
		std::vector<EdgeSharedPtr> edges = resolve_edges(mnet,edge_matrix);
		if (edges.size() != values.size() && values.size()!=1)
			stop("wrong number of values");
		int i=0;
		for (EdgeSharedPtr edge: edges) {
			AttributeStoreSharedPtr store = mnet->edge_features(edge->v1->layer,edge->v2->layer);
			AttributeSharedPtr att = store->attribute(attribute_name);
			if (!att) {
				stop("cannot find attribute: " + attribute_name + " for edges on layers " + edge->v1->layer->name + ", " + edge->v2->layer->name);
			}
			switch (att->type()) {
			case NUMERIC_TYPE:
				if (values.size()==1)
					store->setNumeric(edge->id,att->name(),as<double>(values[0]));
				else
					store->setNumeric(edge->id,att->name(),as<double>(values[i]));
				break;
			case STRING_TYPE:
				if (values.size()==1)
					store->setString(edge->id,att->name(),as<std::string>(values[0]));
				else
					store->setString(edge->id,att->name(),as<std::string>(values[i]));
				break;
			}
			i++;
		}
	}
	else {
		stop("Required at least one parameter: \"actors\", \"layers\", \"nodes\" or \"edges\"");
	}
}

// TRANSFORMATION

void flatten(RMLNetwork& rmnet, const std::string& new_layer, const CharacterVector& layer_names, const std::string& method, bool force_directed, bool all_actors) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();

    std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);

    if (method=="weighted")
        flatten_weighted(mnet,new_layer,layers,force_directed,all_actors);
    else if (method=="or")
        flatten_unweighted(mnet,new_layer,layers,force_directed,all_actors);
    else stop("Unexpected value: method");
}

void project(RMLNetwork& rmnet, const std::string& new_layer, const std::string& layer_name1, const std::string& layer_name2, const std::string& method) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	LayerSharedPtr layer1 = mnet->get_layer(layer_name1);
	LayerSharedPtr layer2 = mnet->get_layer(layer_name2);
	if (!layer1 || !layer2)
		stop("Layer not found");
	if (method=="clique")
		project_unweighted(mnet,new_layer,layer1,layer2);
    else stop("Unexpected value: algorithm");
}

// MEASURES

NumericVector degree_ml(const RMLNetwork& rmnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();

    std::vector<ActorSharedPtr> actors = resolve_actors(mnet,actor_names);
    std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);
    NumericVector res(0);
    for (ActorSharedPtr actor: actors) {
        long deg = 0;
        edge_mode mode = resolve_mode(type);
        deg = degree(mnet,actor,layers,mode);
        if (deg==0) {
            // check if the actor is missing from all layer_names
            bool is_missing = true;
            for (LayerSharedPtr layer: layers) {
                if (mnet->get_node(actor,layer))
                    is_missing = false;
            }
            if (is_missing) res[actor->name] = NA_REAL;
            else res[actor->name] = 0;
        }
        else res[actor->name] = deg;
    }
    return res;
}

NumericVector degree_deviation_ml(const RMLNetwork& rmnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();

    std::vector<ActorSharedPtr> actors = resolve_actors(mnet,actor_names);
    std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);
    NumericVector res(0);
    for (ActorSharedPtr actor: actors) {
        double deg = 0;
        edge_mode mode = resolve_mode(type);
        deg = degree_deviation(mnet,actor,layers,mode);
        if (deg==0) {
            // check if the actor is missing from all layer_names
            bool is_missing = true;
            for (LayerSharedPtr layer: layers) {
                if (mnet->get_node(actor,layer))
                    is_missing = false;
            }
            if (is_missing) res[actor->name] = NA_REAL;
            else res[actor->name] = 0;
        }
        else res[actor->name] = deg;
    }
    return res;
}

NumericVector occupation_ml(const RMLNetwork& rmnet, const NumericMatrix& transitions, double teleportation, long steps) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();

    if (steps==0) {
    	// completely arbitrary value :)
    	steps = 100*mnet->get_edges()->size();
    }
    if (transitions.nrow()!=transitions.ncol()) {
    	stop("expected NxN matrix");
    }
    if (transitions.nrow()!=mnet->get_layers()->size()) {
    	stop("dimensions of transition probability matrix do not match the number of layers in the network");
    }
    matrix<double> m(transitions.nrow());
	for (int i=0; i<transitions.nrow(); i++) {
		std::vector<double> row(transitions.ncol());
		for (int j=0; j<transitions.ncol(); j++) {
			row[j] = transitions(i,j);
		}
		m[i] = row;
	}

    NumericVector res(0);
    std::unordered_map<ActorSharedPtr, int > occ = occupation(mnet,teleportation,m,steps);

	for (const auto &p : occ) {
		res[p.first->name] = p.second;
	}
    return res;
}


NumericVector neighborhood_ml(const RMLNetwork& rmnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();

    std::vector<ActorSharedPtr> actors = resolve_actors(mnet,actor_names);
    std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);
    NumericVector res(0);
    for (ActorSharedPtr actor: actors) {
        long neigh = 0;
        edge_mode mode = resolve_mode(type);
        neigh = neighbors(mnet,actor,layers,mode)->size();
        if (neigh==0) {
            // check if the actor is missing from all layer_names
            bool is_missing = true;
            for (LayerSharedPtr layer: layers) {
                if (mnet->get_node(actor,layer))
                    is_missing = false;
            }
            if (is_missing) res[actor->name] = NA_REAL;
            else res[actor->name] = 0;
        }
        else res[actor->name] = neigh;
    }
    return res;
}



NumericVector xneighborhood_ml(const RMLNetwork& rmnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();

    std::vector<ActorSharedPtr> actors = resolve_actors(mnet,actor_names);
    std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);
    NumericVector res(0);
    for (ActorSharedPtr actor: actors) {
        long neigh = 0;
        edge_mode mode = resolve_mode(type);
        neigh = xneighbors(mnet,actor,layers,mode)->size();
        if (neigh==0) {
            // check if the actor is missing from all layer_names
            bool is_missing = true;
            for (LayerSharedPtr layer: layers) {
                if (mnet->get_node(actor,layer))
                    is_missing = false;
            }
            if (is_missing) res[actor->name] = NA_REAL;
            else res[actor->name] = 0;
        }
        else res[actor->name] = neigh;
    }
    return res;
}


NumericVector connective_redundancy_ml(const RMLNetwork& rmnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();

    std::vector<ActorSharedPtr> actors = resolve_actors(mnet,actor_names);
    std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);
    NumericVector res(0);
    double cr = 0;
    for (ActorSharedPtr actor: actors) {
        edge_mode mode = resolve_mode(type);
        cr = connective_redundancy(mnet,actor,layers,mode);
        if (cr==0) {
            // check if the actor is missing from all layer_names
            bool is_missing = true;
            for (LayerSharedPtr layer: layers) {
                if (mnet->get_node(actor,layer))
                    is_missing = false;
            }
            if (is_missing) res[actor->name] = NA_REAL;
            else res[actor->name] = 0;
        }
        else res[actor->name] = cr;
    }
    return res;
}

NumericVector relevance_ml(const RMLNetwork& rmnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();

    std::vector<ActorSharedPtr> actors = resolve_actors(mnet,actor_names);
    std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);
    NumericVector res(0);
    for (ActorSharedPtr actor: actors) {
        double rel = 0;
        edge_mode mode = resolve_mode(type);
        rel = relevance(mnet,actor,layers,mode);
        if (rel==0) {
            // check if the actor is missing from all layer_names
            bool is_missing = true;
            for (LayerSharedPtr layer: layers) {
                if (mnet->get_node(actor,layer))
                    is_missing = false;
            }
            if (is_missing) res[actor->name] = NA_REAL;
            else res[actor->name] = 0;
        }
        else res[actor->name] = rel;
    }
    return res;
}


NumericVector xrelevance_ml(const RMLNetwork& rmnet, const CharacterVector& actor_names, const CharacterVector& layer_names, const std::string& type) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();

    std::vector<ActorSharedPtr> actors = resolve_actors(mnet,actor_names);
    std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);
    NumericVector res(0);
    for (ActorSharedPtr actor: actors) {
        double rel = 0;
        edge_mode mode = resolve_mode(type);
        rel = xrelevance(mnet,actor,layers,mode);
        if (rel==0) {
            // check if the actor is missing from all layer_names
            bool is_missing = true;
            for (LayerSharedPtr layer: layers) {
                if (mnet->get_node(actor,layer))
                    is_missing = false;
            }
            if (is_missing) res[actor->name] = NA_REAL;
            else res[actor->name] = 0;
        }
        else res[actor->name] = rel;
    }
    return res;
}

double similarity_ml(const RMLNetwork& rmnet, const std::string& layer_name1, const std::string& layer_name2, const std::string& method) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	LayerSharedPtr layer1 = mnet->get_layer(layer_name1);
	if (!layer1) {
		stop("no layer named " + layer_name1);
	}
	LayerSharedPtr layer2 = mnet->get_layer(layer_name2);
	if (!layer2) {
		stop("no layer named " + layer_name2);
	}
	if (method=="jaccard.actors") {
		property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet);
		return jaccard(P,layer1,layer2);
    }
	else if (method=="coverage.actors") {
		property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet);
    	return coverage(P,layer1,layer2);
    }
	else if (method=="kulczynski2.actors") {
		property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet);
    	return kulczynski2(P,layer1,layer2);
    }
	else if (method=="sm.actors") {
		property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet);
    	return simple_matching(P,layer1,layer2);
    }
	else if (method=="rr.actors") {
		property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet);
    	return russell_rao(P,layer1,layer2);
    }
	else if (method=="jaccard.edges") {
		property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet);
		return jaccard(P,layer1,layer2);
    }
	else if (method=="coverage.edges") {
		property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet);
    	return coverage(P,layer1,layer2);
    }
	else if (method=="kulczynski2.edges") {
		property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet);
    	return kulczynski2(P,layer1,layer2);
    }
	else if (method=="sm.edges") {
		property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet);
    	return simple_matching(P,layer1,layer2);
    }
	else if (method=="rr.edges") {
		property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet);
    	return russell_rao(P,layer1,layer2);
    }
	else if (method=="jaccard.triangles") {
    	property_matrix<triad,LayerSharedPtr,bool> P = triangle_existence_property_matrix(mnet);
		return jaccard(P,layer1,layer2);
    }
	else if (method=="coverage.triangles") {
    	property_matrix<triad,LayerSharedPtr,bool> P = triangle_existence_property_matrix(mnet);
    	return coverage(P,layer1,layer2);
    }
	else if (method=="kulczynski2.triangles") {
    	property_matrix<triad,LayerSharedPtr,bool> P = triangle_existence_property_matrix(mnet);
    	return kulczynski2(P,layer1,layer2);
    }
	else if (method=="sm.triangles") {
    	property_matrix<triad,LayerSharedPtr,bool> P = triangle_existence_property_matrix(mnet);
    	return simple_matching(P,layer1,layer2);
    }
	else if (method=="rr.triangles") {
    	property_matrix<triad,LayerSharedPtr,bool> P = triangle_existence_property_matrix(mnet);
    	return russell_rao(P,layer1,layer2);
    }
    else stop("Unexpected value: method parameter");
    return 0;
}

double correlation_ml(const RMLNetwork& rmnet, const std::string& layer_name1, const std::string& layer_name2, const std::string& method, const std::string& type) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	LayerSharedPtr layer1 = mnet->get_layer(layer_name1);
	if (!layer1) {
		stop("no layer named " + layer_name1);
	}
	LayerSharedPtr layer2 = mnet->get_layer(layer_name2);
	if (!layer2) {
		stop("no layer named " + layer_name2);
	}
	if (method=="pearson.degree") {
    	edge_mode mode = resolve_mode(type);
    	property_matrix<ActorSharedPtr,LayerSharedPtr,double> P = actor_degree_property_matrix(mnet,mode);
    	return pearson(P,layer1,layer2);
    }
    if (method=="rho.degree") {
    	edge_mode mode = resolve_mode(type);
    	property_matrix<ActorSharedPtr,LayerSharedPtr,double> P = actor_degree_property_matrix(mnet,mode);
    	P.rankify();
    	return pearson(P,layer1,layer2);
    }
    else stop("Unexpected value: method parameter");
    return 0;
}

GenericMatrix distance_ml(const RMLNetwork& rmnet, const std::string& from, const CharacterVector& to, const std::string& method) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	std::vector<ActorSharedPtr> actors = resolve_actors(mnet,to);
	ActorSharedPtr actor_from = mnet->get_actor(from);
	if (!actor_from) {
		stop("no actor named " + from);
	}
	if (method=="multiplex") {
		std::unordered_map<ActorSharedPtr,std::set<path_length> > dists = pareto_distance(mnet, actor_from);

		int num_rows=0;

		for (ActorSharedPtr actor: actors) {
			num_rows += dists[actor].size();
		}
		int num_cols = mnet->get_layers()->size()+2;
		GenericMatrix res(num_rows,num_cols);

		CharacterVector rownames(0);
		CharacterVector colnames(num_cols);
		colnames(0) = "From";
		colnames(1) = "To";
		for (int i=2; i<num_cols; i++) {
			colnames(i) = mnet->get_layers()->get_at_index(i-2)->name;
		}
		res.attr("dimnames") = List::create(rownames, colnames);

		int current_row=0;
		for (ActorSharedPtr actor: actors) {
			if (dists[actor].size()==0) {
				res(current_row,0) = from;
				res(current_row,1) = actor->name;
				for (int j=2; j<num_cols; j++)
					res(current_row,j) = NA_REAL;
				current_row++;
			}
			for (path_length d: dists[actor]) {
				res(current_row,0) = from;
				res(current_row,1) = actor->name;
				for (int j=2; j<num_cols; j++)
					res(current_row,j) = d.length(mnet->get_layers()->get_at_index(j-2));
				current_row++;
			}
		}
		return res;
	}
	else stop("Unexpected value: method");
	return 0;
}


NumericMatrix sir_ml(const RMLNetwork& rmnet, double beta, int tau, long num_iterations) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	matrix<long> stats = sir(mnet, beta, tau, num_iterations);

	NumericMatrix res(3,num_iterations+1);

	CharacterVector colnames(0);
	CharacterVector rownames(3);
	rownames(0) = "S";
	rownames(1) = "I";
	rownames(2) = "R";
	res.attr("dimnames") = List::create(rownames, colnames);

	for (int i=0; i<3; i++) {
		for (long j=0; j<num_iterations+1; j++) {
			res(i,j) = stats[i][j];
		}
	}
	return res;
}

// COMMUNITY DETECTION

/*/FIXME: m3 not used
GenericMatrix cliquepercolation_ml(const RMLNetwork& rmnet, int k, int m1, int m2, int m3) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();

	hash_set<CommunitySharedPtr> communities = ml_cpm(mnet, k, m1, m2, m3);

	int num_rows = 0;
	int num_cols = 3;
	for (CommunitySharedPtr comm: communities) {
		num_rows += comm->size()*comm->layers.size();
	}
	GenericMatrix res(num_rows,num_cols);
	CharacterVector rownames(0);
	CharacterVector colnames(num_cols);
	colnames(0) = "Actor";
	colnames(1) = "Layer";
	colnames(2) = "Community";
	res.attr("dimnames") = List::create(rownames, colnames);

	int current_row=0;
	int comm_id = 0;
	for (CommunitySharedPtr comm: communities) {
		for (LayerSharedPtr layer: comm->layers) {
			for (ActorSharedPtr actor: comm->actors()) {
				res(current_row,0) = actor->name;
				res(current_row,1) = layer->name;
				res(current_row,2) = comm_id;
				current_row++;
			}
		}
		comm_id++;
	}
	return res;
}*/

// LAYOUT

List multiforce_ml(const RMLNetwork& rmnet, const CharacterVector& layer_names, const NumericVector& w_in, const NumericVector& w_inter, int iterations) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	std::vector<LayerSharedPtr> layers = resolve_layers(mnet,layer_names);
	hash_map<LayerSharedPtr,double> weight_in, weight_inter;
	for (int i=0; i<layers.size(); i++) {
		weight_in[layers.at(i)] = w_in[i];
		weight_inter[layers.at(i)] = w_inter[i];
	}
	hash_map<NodeSharedPtr,xyz_coordinates> coord = multiforce(mnet, 10, 10, weight_in, weight_inter, iterations);
	int num_rows = mnet->get_nodes()->size();
    CharacterVector actor_n(num_rows);
    CharacterVector layer_n(num_rows);
    NumericVector x_n(num_rows);
    NumericVector y_n(num_rows);
    NumericVector z_n(num_rows);
	int current_row=0;
	for (LayerSharedPtr layer: *mnet->get_layers()) {
		for (NodeSharedPtr node: *mnet->get_nodes(layer)) {
			actor_n(current_row) = node->actor->name;
			layer_n(current_row) = node->layer->name;
			xyz_coordinates c = coord.at(node);
			x_n(current_row) = c.x;
			y_n(current_row) = c.y;
			z_n(current_row) = c.z;
			current_row++;
		}
	}
	DataFrame nodes = DataFrame::create(
                             Named("actor")=actor_n,
                             Named("layer")=layer_n,
                             Named("x")=x_n,
                             Named("y")=y_n,
                             Named("z")=z_n);
    
    
    num_rows = mnet->get_edges()->size()*2;
    CharacterVector actor_e(num_rows);
    CharacterVector layer_e(num_rows);
    NumericVector x_e(num_rows);
    NumericVector y_e(num_rows);
    NumericVector z_e(num_rows);
    LogicalVector dir(num_rows);
    current_row=0;
    for (EdgeSharedPtr edge: *mnet->get_edges()) {
        xyz_coordinates c1 = coord.at(edge->v1);
        actor_e(current_row) = edge->v1->actor->name;
        layer_e(current_row) = edge->v1->layer->name;
        x_e(current_row) = c1.x;
        y_e(current_row) = c1.y;
        z_e(current_row) = c1.z;
        dir(current_row) = (edge->directionality==DIRECTED)?true:false;
        current_row++;
        xyz_coordinates c2 = coord.at(edge->v2);
        actor_e(current_row) = edge->v2->actor->name;
        layer_e(current_row) = edge->v2->layer->name;
        x_e(current_row) = c2.x;
        y_e(current_row) = c2.y;
        z_e(current_row) = c2.z;
        dir(current_row) = (edge->directionality==DIRECTED)?true:false;
        current_row++;
    }
    DataFrame edges = DataFrame::create(
                                        Named("actor")=actor_e,
                                        Named("layer")=layer_e,
                                        Named("x")=x_e,
                                        Named("y")=y_e,
                                        Named("z")=z_e,
                                        Named("directed")=dir);
    return List::create(
                        Named("nodes")=nodes,
                        Named("edges")=edges);
}

// Pre-processing
/*
void flatten_w(RMLNetworkSharedPtr& rmnet, const std::string& new_layer_name, const CharacterVector& layer_names, bool force_directed, bool force_actors) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);
	if (method=="weighted") {
		flatten_weighted(mnet,new_layer_name,layers,force_directed, force_actors);
	}
	else if (method=="unweighted") {
		flatten_unweighted(mnet,new_layer_name,layers,force_directed, force_actors);
	}
	else stop("Unexpected value: method parameter");
}

void flatten_or(RMLNetworkSharedPtr& rmnet, const std::string& new_layer_name, const CharacterVector& layers, bool force_directed, bool force_actors) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);
	flatten_unweighted(mnet,new_layer_name,layers,force_directed, force_actors);
}

void project_or(RMLNetworkSharedPtr& rmnet, const std::string& new_layer_name, const std::string& layer_name1, const std::string& layer_name2) {
	MLNetworkSharedPtr mnet = rmnet.get_mlnet();
	LayerSharedPtr layer1 = mnet->get_layer(layer_name1);
	if (!layer1) stop("cannot find layer " + layer_name1);
	LayerSharedPtr layer2 = mnet->get_layer(layer_name2);
	if (!layer2) stop("cannot find layer " + layer_name2);
	project_unweighted(mnet,new_layer_name,layer1,layer2);
}
*/
/*

// MEASURE DISTRIBUTIONS

NumericVector degree_dist(const RMLNetwork& rmnet, const CharacterVector& layer_names, const std::string& type) {
	int max = 0;
	int min = mnet->getNumGlobalIdentities()*mnet.getNumNetworks();
	std::map<int,long> degree_count;
	NumericVector degrees = degree(mnet,actor_names(mnet,layer_names),layer_names,type);
	for (int i=0; i<degrees.size(); ++i) {
		int degree = int(degrees[i]);
		if (degree_count.count(degree)==0) {
			if (max < degree) max=degree;
			if (min > degree) min=degree;
			degree_count[degree] = 0;
		}
		degree_count[degree] = degree_count[degree]+1;
	}
	NumericVector res(max+1);
	for (int i=0; i<res.size(); ++i) {
		if (degree_count.count(i)==0)
			res[i]=0;
		else res[i]=degree_count[i];
	}
	return res;
}


NumericVector neighborhood_dist(const RMLNetwork& rmnet,  const CharacterVector& layer_names, const std::string& type) {
	int max = 0;
	int min = mnet->getNumGlobalIdentities()*mnet.getNumNetworks();
	std::map<int,long> neighborhoods_count;
	NumericVector neighborhoods = neighborhood(mnet,actor_names(mnet,layer_names),layer_names,type);
	for (int i=0; i<neighborhoods.size(); ++i) {
		int neighborhood = int(neighborhoods[i]);
		if (neighborhoods_count.count(neighborhood)==0) {
			if (max < neighborhood) max=neighborhood;
			if (min > neighborhood) min=neighborhood;
			neighborhoods_count[neighborhood] = 0;
		}
		neighborhoods_count[neighborhood] = neighborhoods_count[neighborhood]+1;
	}
	NumericVector res(max+1);
	for (int i=0; i<res.size(); ++i) {
		if (neighborhoods_count.count(i)==0)
			res[i]=0;
		else res[i]=neighborhoods_count[i];
	}
	return res;
}


NumericVector xneighborhood_dist(const RMLNetwork& rmnet,  const CharacterVector& layer_names, const std::string& type) {
	int max = 0;
	int min = mnet->getNumGlobalIdentities()*mnet.getNumNetworks();
	std::map<int,long> neighborhoods_count;
	NumericVector neighborhoods = xneighborhood(mnet,actor_names(mnet,layer_names),layer_names,type);
	for (int i=0; i<neighborhoods.size(); ++i) {
		int neighborhood = int(neighborhoods[i]);
		if (neighborhoods_count.count(neighborhood)==0) {
			if (max < neighborhood) max=neighborhood;
			if (min > neighborhood) min=neighborhood;
			neighborhoods_count[neighborhood] = 0;
		}
		neighborhoods_count[neighborhood] = neighborhoods_count[neighborhood]+1;
	}
	NumericVector res(max+1);
	for (int i=0; i<res.size(); ++i) {
		if (neighborhoods_count.count(i)==0)
			res[i]=0;
		else res[i]=neighborhoods_count[i];
	}
	return res;

}

*/
