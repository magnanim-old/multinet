#include "io.h"

using namespace std;

namespace mlnet {

MLNetworkSharedPtr read_dl(const std::string& infile, const string& network_name, const vector<bool>& symmetric, const vector<bool>& valued) {
	MLNetworkSharedPtr mnet = MLNetwork::create(network_name);
	vector<ActorSharedPtr> actors;
	vector<LayerSharedPtr> layers;
	CSVReader csv;
	csv.trimFields(true);
	csv.setFieldSeparator(' ');
	csv.open(infile);
	vector<string> v = csv.getNext();
	if (v.at(0)!="DL") {
		throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": the file must start with DL");;
	}
	v = csv.getNext();
	string field = v.at(0);
	if (field.substr(0,field.find("="))!="N"){
		throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": expected N, found " + field.substr(0,field.find("=")));;
	}
	size_t num_actors = to_int(field.substr(field.find("=")+1));
	size_t num_layers;
	if (v.size()==1) {
		num_layers = 1;
	}
	else {
		field = v.at(1);
		if (field.substr(0,field.find("="))!="NM"){
			throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": expected NM, found " + field.substr(0,field.find("=")));;
		}
		num_layers = to_int(field.substr(field.find("=")+1));
	}

	csv.getNext(); // FORMAT
	csv.getNext(); // ROW_LABELS:

	for (size_t i=0; i<num_actors; i++) {
		v = csv.getNext();
		if (v.size()==1 && v.at(0)=="") {
			continue;
		}
		actors.push_back(mnet->add_actor(v.at(0)));
	}

	csv.getNext(); // COLUMN_LABELS:

	for (size_t i=0; i<num_actors; i++) csv.getNext();

	csv.getNext(); // LEVEL LABELS:


	for (size_t i=0; i<num_layers; i++) {
		v = csv.getNext();
		if (v.size()==1 && v.at(0)=="") {
			continue;
		}
		LayerSharedPtr layer = mnet->add_layer(v.at(0),symmetric.at(i)?UNDIRECTED:DIRECTED);
		layers.push_back(layer);
		if (valued.at(i)) mnet->edge_features(layer,layer)->add(DEFAULT_WEIGHT_ATTR_NAME,NUMERIC_TYPE);
	}

	// create the corresponding nodes
	for (size_t l=0; l<num_layers; l++) {
		for (size_t i=0; i<num_actors; i++) {
			mnet->add_node(actors.at(i),layers.at(l));
		}
	}

	csv.getNext(); // DATA:

	for (size_t l=0; l<num_layers; l++) {
		for (size_t a1=0; a1<num_actors; a1++) {
			v = csv.getNext();
			for (size_t i=0; i<v.size(); i++) {
				if (v.at(i)=="") {
					v.erase(v.begin()+i);
					i--;
				}
			}
			if (v.size()!=num_actors) {
				throw WrongFormatException("Line " + to_string(csv.rowNum()) + ": wrong number of columns in the data matrix");
			}
			for (size_t a2=0; a2<num_actors; a2++) {
				int val = to_int(v.at(a2));
				if (val==0) continue;
				else if (val>=1) {
					EdgeSharedPtr edge = mnet->add_edge(mnet->get_node(actors.at(a1),layers.at(l)),
							mnet->get_node(actors.at(a2),layers.at(l)));
					if (valued.at(l)) mnet->set_weight(edge->v1,edge->v2,val);
				}

			}
		}
	}

	return mnet;
}
}
