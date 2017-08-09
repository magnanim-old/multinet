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

RMLNetwork readMultilayer(const std::string& input_file, const std::string& name, char sep, bool node_aligned) {
    return RMLNetwork(read_multilayer(input_file,name,sep,node_aligned));
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
    std::vector<double> pr_int(pr_internal_event.size());
    for (size_t i=0; i<pr_internal_event.size(); i++)
        pr_int[i] = pr_internal_event.at(i);
    std::vector<double> pr_ext(pr_external_event.size());
    for (size_t i=0; i<pr_external_event.size(); i++)
        pr_ext[i] = pr_external_event.at(i);
    std::vector<std::vector<double> > dep(dependency.nrow());
    for (size_t i=0; i<dependency.nrow(); i++) {
        std::vector<double> row(dependency.ncol());
        for (size_t j=0; j<dependency.ncol(); j++) {
            row[j] = dependency(i,j);
        }
        dep[i] = row;
    }
    std::vector<EvolutionModelSharedPtr> models(evolution_model.size());
    for (size_t i=0; i<models.size(); i++) {
        models[i] = (as<REvolutionModel>(evolution_model[i])).get_model();
    }
    return RMLNetwork(evolve(num_of_steps,num_actors,pr_int,pr_ext,dep,models));
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

DataFrame nodes(const RMLNetwork& rmnet, const CharacterVector& layer_names) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    std::unordered_set<LayerSharedPtr> layers = resolve_layers_unordered(mnet,layer_names);
    CharacterVector actor, layer;
    for (NodeSharedPtr node: *mnet->get_nodes()) {
        if (layers.count(node->layer)==0) continue;
        actor.push_back(node->actor->name);
        layer.push_back(node->layer->name);
    }
    return DataFrame::create(_["actor"] = actor, _["layer"] = layer);
}

DataFrame edges_idx(const RMLNetwork& rmnet) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    NumericVector from, to, directed;
    NodeListSharedPtr nodes = mnet->get_nodes();
    EdgeListSharedPtr edges = mnet->get_edges();
    for (EdgeSharedPtr edge: *edges) {
        from.push_back(nodes->get_index(edge->v1)+1);
        to.push_back(nodes->get_index(edge->v2)+1);
        directed=(edge->directionality==DIRECTED)?1:0;
    }
    return DataFrame::create(_["from"] = from, _["to"] = to, _["dir"] = directed );
}

DataFrame edges(const RMLNetwork& rmnet, const CharacterVector& layer_names1, const CharacterVector& layer_names2) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    std::vector<LayerSharedPtr> layers1 = resolve_layers(mnet,layer_names1);
    std::vector<LayerSharedPtr> layers2;
    if (layer_names2.size()==0)
        layers2 = layers1;
    else layers2 = resolve_layers(mnet,layer_names2);
    CharacterVector from_a, from_l, to_a, to_l;
    NumericVector directed;
    for (LayerSharedPtr layer1: layers1) {
        for (LayerSharedPtr layer2: layers2) {
            if (layer1>layer2 && !mnet->is_directed(layer1,layer2))
                continue;
            for (EdgeSharedPtr edge: *mnet->get_edges(layer1,layer2)) {
                from_a.push_back(edge->v1->actor->name);
                from_l.push_back(layer1->name);
                to_a.push_back(edge->v2->actor->name);
                to_l.push_back(layer2->name);
                directed.push_back((edge->directionality==DIRECTED)?1:0);
            }
        }
    }
    return DataFrame::create(_["from_actor"] = from_a, _["from_layer"] = from_l, _["to_actor"] = to_a, _["to_layer"] = to_l, _["dir"] = directed );
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

DataFrame isDirected(const RMLNetwork& rmnet, const CharacterVector& layer_names1, const CharacterVector& layer_names2) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    std::vector<LayerSharedPtr> layers1 = resolve_layers(mnet,layer_names1);
    std::vector<LayerSharedPtr> layers2;
    if (layer_names2.size()==0)
        layers2 = layers1;
    else layers2 = resolve_layers(mnet,layer_names2);
    
    CharacterVector l1, l2;
    NumericVector directed;
    for (LayerSharedPtr layer1: layers1) {
        for (LayerSharedPtr layer2: layers2) {
            l1.push_back(layer1->name);
            l2.push_back(layer2->name);
            directed.push_back((mnet->is_directed(layer1,layer2))?1:0);
        }
    }
    return DataFrame::create(_["layer1"] = l1, _["layer2"] = l2, _["dir"] = directed );
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
        for (size_t i=0; i<layer_names.size(); i++) {
            mnet->add_layer(std::string(layer_names[i]),directed[0]?DIRECTED:UNDIRECTED);
        }
    }
    else if (layer_names.size()!=directed.size())
        stop("Same number of layer names and layer directionalities expected");
    else {
        for (size_t i=0; i<layer_names.size(); i++) {
            mnet->add_layer(std::string(layer_names[i]),directed[i]?DIRECTED:UNDIRECTED);
        }
    }
}

void addActors(RMLNetwork& rmnet, const CharacterVector& actor_names) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    for (size_t i=0; i<actor_names.size(); i++) {
        mnet->add_actor(std::string(actor_names[i]));
    }
}

void addNodes(RMLNetwork& rmnet, const DataFrame& nodes) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    
    CharacterVector a = nodes(0);
    CharacterVector l = nodes(1);

    for (size_t i=0; i<nodes.nrow(); i++) {
        ActorSharedPtr actor = mnet->get_actor(std::string(a(i)));
        if (!actor) stop("cannot find actor " + std::string(a(i)));
        LayerSharedPtr layer = mnet->get_layer(std::string(l(i)));
        if (!layer) stop("cannot find layer " + std::string(l(i)));
        mnet->add_node(actor,layer);
    }
}

void addEdges(RMLNetwork& rmnet, const DataFrame& edges) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    
    CharacterVector a_from = edges(0);
    CharacterVector l_from = edges(1);
    CharacterVector a_to = edges(2);
    CharacterVector l_to = edges(3);

    for (size_t i=0; i<edges.nrow(); i++) {
        ActorSharedPtr actor1 = mnet->get_actor(std::string(a_from(i)));
        if (!actor1) stop("cannot find actor " + std::string(a_from(i)));
        ActorSharedPtr actor2 = mnet->get_actor(std::string(a_to(i)));
        if (!actor2) stop("cannot find actor " + std::string(a_to(i)));
        LayerSharedPtr layer1 = mnet->get_layer(std::string(l_from(i)));
        if (!layer1) stop("cannot find layer " + std::string(l_from(i)));
        LayerSharedPtr layer2 = mnet->get_layer(std::string(l_to(i)));
        if (!layer2) stop("cannot find layer " + std::string(l_to(i)));
        NodeSharedPtr node1 = mnet->get_node(actor1,layer1);
        if (!node1) stop("cannot find node " + actor1->name + " " + layer1->name);
        NodeSharedPtr node2 = mnet->get_node(actor2,layer2);
        if (!node2) stop("cannot find node " + actor2->name + " " + layer2->name);
        mnet->add_edge(node1,node2);
    }
}

void setDirected(const RMLNetwork& rmnet, const DataFrame& layers_dir) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    CharacterVector l1 = layers_dir(0);
    CharacterVector l2 = layers_dir(1);
    NumericVector dir = layers_dir(2);
    for (size_t i=0; i<layers_dir.nrow(); i++) {
        LayerSharedPtr layer1 = mnet->get_layer(std::string(l1(i)));
        if (!layer1) stop("cannot find layer " + std::string(l1(i)));
        LayerSharedPtr layer2 = mnet->get_layer(std::string(l2(i)));
        if (!layer2) stop("cannot find layer " + std::string(l2(i)));
        int directed = (int)dir(i);
        if (directed!=0 && directed!=1) stop("directionality can only be 0 or 1");
        mnet->set_directed(layer1,layer2,directed);
    }
}

void deleteLayers(RMLNetwork& rmnet, const CharacterVector& layer_names) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    for (size_t i=0; i<layer_names.size(); i++) {
        LayerSharedPtr layer = mnet->get_layer(std::string(layer_names(i)));
        mnet->erase(layer);
    }
}

void deleteActors(RMLNetwork& rmnet, const CharacterVector& actor_names) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    for (size_t i=0; i<actor_names.size(); i++) {
        ActorSharedPtr actor = mnet->get_actor(std::string(actor_names(i)));
        mnet->erase(actor);
    }
}

void deleteNodes(RMLNetwork& rmnet, const DataFrame& node_matrix) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    std::vector<NodeSharedPtr> nodes = resolve_nodes(mnet,node_matrix);
    
    for (NodeSharedPtr node: nodes) {
        mnet->erase(node);
    }
}

void deleteEdges(RMLNetwork& rmnet, const DataFrame& edge_matrix) {
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
        for (size_t i=0; i<attribute_names.size(); i++) {
            mnet->actor_features()->add(std::string(attribute_names[i]),a_type);
        }
    }
    else if (target=="layer") {
        if (layer_name!="" || layer_name1!="" || layer_name2!="")
            stop("No layers should be specified for target 'layer'");
        for (size_t i=0; i<attribute_names.size(); i++) {
            mnet->layer_features()->add(std::string(attribute_names[i]),a_type);
        }
    }
    else if (target=="node") {
        if (layer_name1!="" || layer_name2!="")
            stop("layer1 and layer2 should not be specified for target 'node'");
        LayerSharedPtr layer = mnet->get_layer(layer_name);
        if (!layer) stop("layer " + layer_name + " not found");
        for (size_t i=0; i<attribute_names.size(); i++) {
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
        for (size_t i=0; i<attribute_names.size(); i++) {
            mnet->edge_features(layer1,layer2)->add(std::string(attribute_names[i]),a_type);
        }
    }
    else stop("wrong target: " + target);
}

DataFrame getAttributes(const RMLNetwork& rmnet, const std::string& target) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    if (target=="actor") {
        AttributeStoreSharedPtr store = mnet->actor_features();
        CharacterVector a_name, a_type;
        for (AttributeSharedPtr att: store->attributes()) {
            a_name.push_back(att->name());
            a_type.push_back(att->type_as_string());
        }
        return DataFrame::create(_["name"] = a_name, _["type"] = a_type);
    }
    else if (target=="layer") {
        AttributeStoreSharedPtr store = mnet->layer_features();
        CharacterVector a_name, a_type;
        for (AttributeSharedPtr att: store->attributes()) {
            a_name.push_back(att->name());
            a_type.push_back(att->type_as_string());
        }
        return DataFrame::create(_["name"] = a_name, _["type"] = a_type);
    }
    else if (target=="node") {
        CharacterVector a_layer, a_name, a_type;
        for (LayerSharedPtr layer: *mnet->get_layers()) {
            AttributeStoreSharedPtr store=mnet->node_features(layer);
            for (AttributeSharedPtr att: store->attributes()) {
                a_layer.push_back(layer->name);
                a_name.push_back(att->name());
                a_type.push_back(att->type_as_string());
            }
        }
        return DataFrame::create(_["layer"] = a_layer, _["name"] = a_name, _["type"] = a_type);
    }
    else if (target=="edge") {
        CharacterVector a_layer1, a_layer2, a_name, a_type;
        for (LayerSharedPtr layer1: *mnet->get_layers()) {
            for (LayerSharedPtr layer2: *mnet->get_layers()) {
                if (!mnet->is_directed(layer1,layer2) && layer1->name>layer2->name) continue;
                AttributeStoreSharedPtr store=mnet->edge_features(layer1,layer2);
                for (AttributeSharedPtr att: store->attributes()) {
                    a_layer1.push_back(layer1->name);
                    a_layer2.push_back(layer2->name);
                    a_name.push_back(att->name());
                    a_type.push_back(att->type_as_string());
                }
            }
        }
        return DataFrame::create(_["layer1"] = a_layer1, _["layer2"] = a_layer2, _["name"] = a_name, _["type"] = a_type);
    }
    else stop("wrong target: " + target);
    return 0; // never gets here
}

DataFrame getValues(RMLNetwork& rmnet, const std::string& attribute_name, const CharacterVector& actor_names, const CharacterVector& layer_names, const DataFrame& node_matrix, const DataFrame& edge_matrix) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    if (actor_names.size() != 0) {
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
        if (att->type()==NUMERIC_TYPE) {
            NumericVector value;
            for (ActorSharedPtr actor: actors) {
                value.push_back(store->getNumeric(actor->id,att->name()));
            }
            return DataFrame::create(_["value"] = value);
        }
        else if (att->type()==STRING_TYPE) {
            CharacterVector value;
            for (ActorSharedPtr actor: actors) {
                value.push_back(store->getString(actor->id,att->name()));
            }
            return DataFrame::create(_["value"] = value);
        }
    }
    else if (layer_names.size() != 0) {
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
        if (att->type()==NUMERIC_TYPE) {
            NumericVector value;
            for (LayerSharedPtr layer: layers) {
                value.push_back(store->getNumeric(layer->id,att->name()));
            }
            return DataFrame::create(_["value"] = value);
        }
        else if (att->type()==STRING_TYPE) {
            CharacterVector value;
            for (LayerSharedPtr layer: layers) {
                value.push_back(store->getString(layer->id,att->name()));
            }
            return DataFrame::create(_["value"] = value);
        }
        
    }
    // local attributes: vertexes
    // must be from a single layer
    else if (node_matrix.size() > 0) {
        if (edge_matrix.size() > 0)
            Rcout << "Warning: unused parameter: \"edges\"" << std::endl;
        std::vector<NodeSharedPtr> nodes = resolve_nodes(mnet,node_matrix);
        
        LayerSharedPtr layer = nodes.at(0)->layer;
        
        AttributeStoreSharedPtr store = mnet->node_features(layer);
        AttributeSharedPtr att = store->attribute(attribute_name);
        if (!att) {
            stop("cannot find attribute: " + attribute_name + " for nodes on layer " + layer->name);
        }
        if (att->type()==NUMERIC_TYPE) {
            NumericVector value;
            for (NodeSharedPtr node: nodes) {
                value.push_back(store->getNumeric(node->id,att->name()));
            }
            return DataFrame::create(_["value"] = value);
        }
        else if (att->type()==STRING_TYPE) {
            CharacterVector value;
            for (NodeSharedPtr node: nodes) {
                value.push_back(store->getString(node->id,att->name()));
            }
            return DataFrame::create(_["value"] = value);
        }
    }
    else if (edge_matrix.size() > 0) {
        std::vector<EdgeSharedPtr> edges = resolve_edges(mnet,edge_matrix);
        LayerSharedPtr layer1 = edges.at(0)->v1->layer;
        LayerSharedPtr layer2 = edges.at(0)->v2->layer;
        AttributeStoreSharedPtr store = mnet->edge_features(layer1,layer2);
        AttributeSharedPtr att = store->attribute(attribute_name);
        if (!att) {
            stop("cannot find attribute: " + attribute_name + " for edges on layers " + layer1->name + ", " + layer2->name);
        }
        if (att->type()==NUMERIC_TYPE) {
            NumericVector value;
            for (EdgeSharedPtr edge: edges) {
                value.push_back(store->getNumeric(edge->id,att->name()));
            }
            return DataFrame::create(_["value"] = value);
        }
        else if (att->type()==STRING_TYPE) {
            CharacterVector value;
            for (EdgeSharedPtr edge: edges) {
                value.push_back(store->getString(edge->id,att->name()));
            }
            return DataFrame::create(_["value"] = value);
        }
    }
    else {
        stop("Required at least one parameter: \"actors\", \"layers\", \"nodes\" or \"edges\"");
    }
    // Never gets here
    return 0;
}

void setValues(RMLNetwork& rmnet, const std::string& attribute_name, const CharacterVector& actor_names, const CharacterVector& layer_names, const DataFrame& node_matrix, const DataFrame& edge_matrix, const GenericVector& values) {
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
        size_t i=0;
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
        size_t i=0;
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
        size_t i=0;
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
        size_t i=0;
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
    for (size_t i=0; i<transitions.nrow(); i++) {
        std::vector<double> row(transitions.ncol());
        for (size_t j=0; j<transitions.ncol(); j++) {
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

DataFrame comparison_ml(const RMLNetwork& rmnet, const CharacterVector& layer_names, const std::string& method, const std::string& type, int K) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    std::vector<LayerSharedPtr> layers = resolve_layers(mnet,layer_names);
    std::vector<NumericVector> values;
    for (size_t i=0; i<layers.size(); i++) {
        NumericVector v;
        values.push_back(v);
    }
    DataFrame res = DataFrame::create();
    if (method=="jaccard.actors") {
        property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(jaccard(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="coverage.actors") {
        property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(coverage(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="kulczynski2.actors") {
        property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(kulczynski2(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="sm.actors") {
        property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(simple_matching(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="rr.actors") {
        property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(russell_rao(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="hamann.actors") {
        property_matrix<ActorSharedPtr,LayerSharedPtr,bool> P = actor_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(hamann(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="jaccard.edges") {
        property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(jaccard(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="coverage.edges") {
        property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(coverage(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="kulczynski2.edges") {
        property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(kulczynski2(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="sm.edges") {
        property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(simple_matching(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="rr.edges") {
        property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(russell_rao(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="hamann.edges") {
        property_matrix<dyad,LayerSharedPtr,bool> P = edge_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(hamann(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="jaccard.triangles") {
        property_matrix<triad,LayerSharedPtr,bool> P = triangle_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(jaccard(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="coverage.triangles") {
        property_matrix<triad,LayerSharedPtr,bool> P = triangle_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(coverage(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="kulczynski2.triangles") {
        property_matrix<triad,LayerSharedPtr,bool> P = triangle_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(kulczynski2(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="sm.triangles") {
        property_matrix<triad,LayerSharedPtr,bool> P = triangle_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(simple_matching(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="rr.triangles") {
        property_matrix<triad,LayerSharedPtr,bool> P = triangle_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(russell_rao(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="hamann.triangles") {
        property_matrix<triad,LayerSharedPtr,bool> P = triangle_existence_property_matrix(mnet);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(hamann(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="dissimilarity.degree") {
        edge_mode mode = resolve_mode(type);
        property_matrix<ActorSharedPtr,LayerSharedPtr,double> P = actor_degree_property_matrix(mnet,mode);
        if (K<=0) K=std::ceil(std::log2(P.num_structures) + 1);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(dissimilarity_index(P,layers[i],layers[j],K));
            }
        }
    }
    else if (method=="KL.degree") {
        edge_mode mode = resolve_mode(type);
        property_matrix<ActorSharedPtr,LayerSharedPtr,double> P = actor_degree_property_matrix(mnet,mode);
        if (K<=0) K=std::ceil(std::log2(P.num_structures) + 1);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(KL_divergence(P,layers[i],layers[j],K));
            }
        }
    }
    else if (method=="jeffrey.degree") {
        edge_mode mode = resolve_mode(type);
        property_matrix<ActorSharedPtr,LayerSharedPtr,double> P = actor_degree_property_matrix(mnet,mode);
        if (K<=0) K=std::ceil(std::log2(P.num_structures) + 1);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(jeffrey_divergence(P,layers[i],layers[j],K));
            }
        }
    }
    else if (method=="pearson.degree") {
        edge_mode mode = resolve_mode(type);
        property_matrix<ActorSharedPtr,LayerSharedPtr,double> P = actor_degree_property_matrix(mnet,mode);
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(pearson(P,layers[i],layers[j]));
            }
        }
    }
    else if (method=="rho.degree") {
        edge_mode mode = resolve_mode(type);
        property_matrix<ActorSharedPtr,LayerSharedPtr,double> P = actor_degree_property_matrix(mnet,mode);
        P.rankify();
        for (size_t j=0; j<layers.size(); j++) {
            for (size_t i=0; i<layers.size(); i++) {
                values[j].push_back(pearson(P,layers[i],layers[j]));
            }
        }
    }
    else stop("Unexpected value: method parameter");
    if (layer_names.size()==0) {
        CharacterVector names;
        for (LayerSharedPtr l: layers)
            names.push_back(l->name);
        for (size_t i=0; i<layers.size(); i++) {
            res.push_back(values[i],std::string(names[i]));
        }
        res.attr("class") = "data.frame";
        res.attr("row.names") = names;
    }
    else {
        for (size_t i=0; i<layers.size(); i++) {
            res.push_back(values[i],std::string(layer_names[i]));
        }
        res.attr("class") = "data.frame";
        res.attr("row.names") = layer_names;
    }
    return res;
}

double summary_ml(const RMLNetwork& rmnet, const std::string& layer_name, const std::string& method, const std::string& type) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    LayerSharedPtr layer = mnet->get_layer(layer_name);
    if (!layer) {
        stop("no layer named " + layer_name);
    }
    edge_mode mode = resolve_mode(type);
    property_matrix<ActorSharedPtr,LayerSharedPtr,double> P = actor_degree_property_matrix(mnet,mode);
    if (method=="min.degree") {
        return min(P,layer);
    }
    else if (method=="min.degree") {
        return min(P,layer);
    }
    else if (method=="max.degree") {
        return max(P,layer);
    }
    else if (method=="sum.degree") {
        return sum(P,layer);
    }
    else if (method=="mean.degree") {
        return mean(P,layer);
    }
    else if (method=="sd.degree") {
        return sd(P,layer);
    }
    else if (method=="skewness.degree") {
        return skew(P,layer);
    }
    else if (method=="kurtosis.degree") {
        return kurt(P,layer);
    }
    else if (method=="entropy.degree") {
        return entropy(P,layer);
    }
    else if (method=="CV.degree") {
        return CV(P,layer);
    }
    else if (method=="jarque.bera.degree") {
        return jarque_bera(P,layer);
    }
    else stop("Unexpected value: method parameter");
    return 0;
}


DataFrame distance_ml(const RMLNetwork& rmnet, const std::string& from_actor, const CharacterVector& to_actors, const std::string& method) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    std::vector<ActorSharedPtr> actors_to = resolve_actors(mnet,to_actors);
    ActorSharedPtr actor_from = mnet->get_actor(from_actor);
    if (!actor_from) {
        stop("no actor named " + from_actor);
    }
    if (method=="multiplex") {
        std::unordered_map<ActorSharedPtr,std::set<path_length> > dists = pareto_distance(mnet, actor_from);
        
        CharacterVector from, to;
        std::vector<NumericVector> lengths;
        for (size_t i=0; i<mnet->get_layers()->size(); i++) {
            NumericVector v;
            lengths.push_back(v);
        }
        
        
        for (ActorSharedPtr actor: actors_to) {
            for (path_length d: dists[actor]) {
                from.push_back(from_actor);
                to.push_back(actor->name);
                for (size_t i=0; i<mnet->get_layers()->size(); i++) {
                    lengths[i].push_back(d.length(mnet->get_layers()->get_at_index(i)));
                }
            }
        }
        DataFrame res = DataFrame::create(_["from"] = from, _["to"] = to);
        for (size_t i=0; i<mnet->get_layers()->size(); i++)
            res.push_back(lengths[i],mnet->get_layers()->get_at_index(i)->name);
        
        return DataFrame(res);
    }
    else stop("Unexpected value: method");
    return 0;
}

/*
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
 
 for (size_t i=0; i<3; i++) {
 for (long j=0; j<num_iterations+1; j++) {
 res(i,j) = stats[i][j];
 }
 }
 return res;
 }*/

// COMMUNITY DETECTION

DataFrame cliquepercolation_ml(const RMLNetwork& rmnet, int k, int m) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    
    CommunityStructureSharedPtr community_structure = mlcpm(mnet, k, m);
    return to_dataframe(community_structure);
}

DataFrame glouvain_ml(const RMLNetwork& rmnet, double gamma, double omega, int limit) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    
    glouvain gl;
    CommunityStructureSharedPtr community_structure = gl.fit(mnet, "move", gamma, omega, limit);
    return to_dataframe(community_structure);
}

DataFrame lart_ml(const RMLNetwork& rmnet, int t, double eps, double gamma) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    
    lart k;
    if (t<0) t = mnet->get_layers()->size()*3;
    CommunityStructureSharedPtr community_structure = k.fit(mnet, t, eps, gamma);
    return to_dataframe(community_structure);
}

DataFrame abacus_ml(const RMLNetwork& rmnet, int min_actors, int min_layers) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    
    try {
        ActorCommunityStructureSharedPtr community_structure = abacus(mnet, min_actors, min_layers);
        return to_dataframe(to_node_communities(community_structure,mnet));
    }
    catch (std::exception& e) {
        Rcout << "Warning: could not run external library: " << e.what() << std::endl;
        Rcout << "Returning empty community set." << std::endl;
    }
    return to_dataframe(to_node_communities(actor_community_structure::create(),mnet));
    
}


List to_list(const DataFrame& cs, const RMLNetwork& rmnet) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    std::map<int, std::map<int, std::vector<int> > > list;
    CharacterVector cs_actor = cs["actor"];
    CharacterVector cs_layer = cs["layer"];
    NumericVector cs_cid = cs["cid"];
    for (size_t i=0; i<cs.nrow(); i++) {
        int comm_id = cs_cid[i];
        LayerSharedPtr layer = mnet->get_layer(std::string(cs_layer[i]));
        int l = mnet->get_layers()->get_index(layer);
        ActorSharedPtr actor = mnet->get_actor(std::string(cs_actor[i]));
        int n = mnet->get_nodes()->get_index(mnet->get_node(actor,layer))+1;
        list[comm_id][l].push_back(n);
    }
    List res = List::create();
    for (auto clist: list) {
        for (auto llist: clist.second) {
            res.push_back(List::create(_["cid"]=clist.first, _["lid"]=llist.first, _["aid"]=llist.second));
        }
    }
    return res;
}


// LAYOUT

DataFrame multiforce_ml(const RMLNetwork& rmnet, const NumericVector& w_in, const NumericVector& w_inter, const NumericVector& gravity, int iterations) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    hash_map<LayerSharedPtr,double> weight_in, weight_inter, weight_gr;
    LayerListSharedPtr layers = mnet->get_layers();
    if (w_in.size()==1) {
        for (size_t i=0; i<layers->size(); i++) {
            weight_in[layers->get_at_index(i)] = w_in[0];
        }
    }
    else if (w_in.size()==layers->size()) {
        for (size_t i=0; i<layers->size(); i++) {
            weight_in[layers->get_at_index(i)] = w_in[i];
        }
    }
    else stop("wrong dimension: internal weights (should contain 1 or num.layers.ml weights)");
    if (w_inter.size()==1) {
        for (size_t i=0; i<layers->size(); i++) {
            weight_inter[layers->get_at_index(i)] = w_inter[0];
        }
    }
    else if (w_inter.size()==layers->size()) {
        for (size_t i=0; i<layers->size(); i++) {
            weight_inter[layers->get_at_index(i)] = w_inter[i];
        }
    }
    else stop("wrong dimension: external weights (should contain 1 or num.layers.ml weights)");
    if (gravity.size()==1) {
        for (size_t i=0; i<layers->size(); i++) {
            weight_gr[layers->get_at_index(i)] = gravity[0];
        }
    }
    else if (gravity.size()==layers->size()) {
        for (size_t i=0; i<layers->size(); i++) {
            weight_gr[layers->get_at_index(i)] = gravity[i];
        }
    }
    else stop("wrong dimension: gravity (should contain 1 or num.layers.ml weights)");
    
    hash_map<NodeSharedPtr,xyz_coordinates> coord = multiforce(mnet, 10, 10, weight_in, weight_inter, weight_gr, iterations);
    int num_rows = mnet->get_nodes()->size();
    CharacterVector actor_n(num_rows);
    CharacterVector layer_n(num_rows);
    NumericVector x_n(num_rows);
    NumericVector y_n(num_rows);
    NumericVector z_n(num_rows);
    int current_row=0;
    for (NodeSharedPtr node: *mnet->get_nodes()) {
        actor_n(current_row) = node->actor->name;
        layer_n(current_row) = node->layer->name;
        xyz_coordinates c = coord.at(node);
        x_n(current_row) = c.x;
        y_n(current_row) = c.y;
        z_n(current_row) = c.z;
        current_row++;
    }
    DataFrame nodes = DataFrame::create(
                                        Named("actor")=actor_n,
                                        Named("layer")=layer_n,
                                        Named("x")=x_n,
                                        Named("y")=y_n,
                                        Named("z")=z_n);
    
    return nodes;
}


DataFrame circular_ml(const RMLNetwork& rmnet) {
    MLNetworkSharedPtr mnet = rmnet.get_mlnet();
    
    hash_map<NodeSharedPtr,xyz_coordinates> coord = circular(mnet, 10.0);
    int num_rows = mnet->get_nodes()->size();
    CharacterVector actor_n(num_rows);
    CharacterVector layer_n(num_rows);
    NumericVector x_n(num_rows);
    NumericVector y_n(num_rows);
    NumericVector z_n(num_rows);
    int current_row=0;
    for (NodeSharedPtr node: *mnet->get_nodes()) {
        actor_n(current_row) = node->actor->name;
        layer_n(current_row) = node->layer->name;
        xyz_coordinates c = coord.at(node);
        x_n(current_row) = c.x;
        y_n(current_row) = c.y;
        z_n(current_row) = c.z;
        current_row++;
    }
    DataFrame nodes = DataFrame::create(
                                        Named("actor")=actor_n,
                                        Named("layer")=layer_n,
                                        Named("x")=x_n,
                                        Named("y")=y_n,
                                        Named("z")=z_n);
    
    return nodes;
}

