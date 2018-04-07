/*
 * Evaluation functions for community detection
 */

#include "community.h"
#include "utils/sortedrandomset.h"
#include <algorithm>

namespace mlnet {

    double community_jaccard(const CommunitySharedPtr& c1, const CommunitySharedPtr& c2) {
        int common_nodes = intersection_size(c1->get_nodes(),c2->get_nodes());
        return (double)common_nodes/(c1->get_nodes().size()+c2->get_nodes().size()-common_nodes);
    }

    /**
     * Returns the agreement between two clusterings com1 and com2
     * @param com1
     * @param com2
     * @param n number of nodes in the original network
     */
    double normalized_mutual_information(const CommunityStructureSharedPtr& com1, const CommunityStructureSharedPtr& com2, int n) {
        double entropy_c1 = 0;

        for (size_t i = 0; i < com1->get_communities().size(); i++) {
            int size1 = com1->get_community(i)->get_nodes().size();
            if (size1 == 0) {
                continue;
            }
            entropy_c1 -= (double)size1/n * std::log2((double)size1/n);
        }

        double entropy_c2 = 0;
        for (size_t j = 0; j < com2->get_communities().size(); j++) {
            int size2 = com2->get_community(j)->get_nodes().size();
            if (size2 == 0){
              continue;
            }
            entropy_c2 -= (double)size2/n * std::log2((double)size2/n);
        }

        double info = 0;
        for (size_t i = 0; i < com1->get_communities().size(); i++) {
            for (size_t j = 0; j < com2->get_communities().size(); j++) {

                int common_nodes = intersection_size(com1->get_community(i)->get_nodes(),com2->get_community(j)->get_nodes());

                int size1 = com1->get_community(i)->get_nodes().size();
                int size2 = com2->get_community(j)->get_nodes().size();

                if (size1==0 || size2==0 || common_nodes==0)
                    continue;
                info += (double)common_nodes/n * std::log2((double)n*common_nodes/(size1*size2));
            }
        }
        return info/((entropy_c1+entropy_c2)/2);
    }
    
    CommunityStructureSharedPtr read_ground_truth(const string& infile, char separator, const MLNetworkSharedPtr& mnet) {
        CSVReader csv;
        csv.trimFields(true);
        csv.setFieldSeparator(separator);
        csv.open(infile);
        hash_map<std::string, std::set<NodeSharedPtr> > result;
        while (csv.hasNext()) {
            vector<string> v = csv.getNext();
            ActorSharedPtr a = mnet->get_actor(v.at(0));
            if (v.size()==3) {
                LayerSharedPtr l = mnet->get_layer(v.at(1));
                NodeSharedPtr n = mnet->get_node(a,l);
                result[v.at(2)].insert(n);
            }
            else if (v.size()==2) {
                for (NodeSharedPtr n: *mnet->get_nodes(a)) {
                    result[v.at(1)].insert(n);
                }
            }
            else throw WrongFormatException("The ground truth file must specify Actor,Layer,CommunityID or Actor,CommunityID");
        }
        
        CommunityStructureSharedPtr communities = community_structure::create();
        
        for (auto pair: result) {
            CommunitySharedPtr c = community::create();
            for (NodeSharedPtr node: pair.second) {
                c->add_node(node);
            }
            communities->add_community(c);
        }
        
        return communities;
    }


    double omega_index(const CommunityStructureSharedPtr& partitioning1, const CommunityStructureSharedPtr& partitioning2,const MLNetworkSharedPtr& mnet){

    	//Create a map to represent pairs agreement in each partitioning
    	//The map is of the form [ key = pair of nodes (node1,node2) and  value = number of times they co-occured together]
      	std::map<std::pair<NodeSharedPtr,NodeSharedPtr>, int> p1_pairs_cooccurance;
      	std::map<std::pair<NodeSharedPtr,NodeSharedPtr>, int> p2_pairs_cooccurance;

      	//Get the nodes of the multi-net instance
      	NodeListSharedPtr network_nodes =  mnet->get_nodes();

      	//Initialise both partitioning maps
      	for (NodeSharedPtr node1:*network_nodes){
      		for (NodeSharedPtr node2:*network_nodes){
      		   if(node1!=node2){
      			   std::pair<NodeSharedPtr,NodeSharedPtr> key (node1 ,node2);
      			   std::pair<NodeSharedPtr,NodeSharedPtr> key_inversed (node2,node1);
      			   if(p1_pairs_cooccurance.find(key)== p1_pairs_cooccurance.end() & p1_pairs_cooccurance.find(key_inversed)==p1_pairs_cooccurance.end()){
      			     p1_pairs_cooccurance[key]=0;
      			     p2_pairs_cooccurance[key]=0;
      			   }
      		   }
      		}
      	}

      	//Extract the communities from each partitioning
      	vector<CommunitySharedPtr> partitioning1_coms = partitioning1->get_communities();
      	vector<CommunitySharedPtr> partitioning2_coms = partitioning2->get_communities();

      	//Iterate through the first partitioning communities to set the values for the corresponding map
          for(CommunitySharedPtr com:partitioning1_coms){
              //For each pair of nodes in the community, increment the corresponding index in the map
              hash_set<NodeSharedPtr> com_nodes = com->get_nodes();
              for (hash_set<NodeSharedPtr>::iterator it1 = com_nodes.begin(); it1 != com_nodes.end(); ++it1) {
                  for (hash_set<NodeSharedPtr>::iterator it2 = std::next(it1); it2 != com_nodes.end(); ++it2) {
                	  //Only if the corresponding nodes are different
                	  if(*it1 != *it2){
						  std::pair<NodeSharedPtr,NodeSharedPtr> key (*it1 ,*it2);
						  std::pair<NodeSharedPtr,NodeSharedPtr> key_inversed (*it2,*it1);

						  std::map<std::pair<NodeSharedPtr,NodeSharedPtr>, int>::const_iterator iter ;
						  iter= p1_pairs_cooccurance.find(key);

						  if(iter!=p1_pairs_cooccurance.end()){
							  p1_pairs_cooccurance[key]=p1_pairs_cooccurance[key]+1;
						  }
						  else{
						  iter= p1_pairs_cooccurance.find(key_inversed);
						  if(iter!=p1_pairs_cooccurance.end()){
							  p1_pairs_cooccurance[key_inversed]=p1_pairs_cooccurance[key_inversed]+1;
							  }
						  }
                    }
                  }
              }
          }

          //Iterate through the second partitioning communities to set the values for the corresponding map
			for(CommunitySharedPtr com:partitioning2_coms){
				//For each pair of nodes in the community, increment the corresponding index in the map
				hash_set<NodeSharedPtr> com_nodes = com->get_nodes();
				for (hash_set<NodeSharedPtr>::iterator it1 = com_nodes.begin(); it1 != com_nodes.end(); ++it1) {
					for (hash_set<NodeSharedPtr>::iterator it2 = std::next(it1); it2 != com_nodes.end(); ++it2) {
					  //Only if the corresponding nodes are different
					  if(*it1 != *it2){
						  std::pair<NodeSharedPtr,NodeSharedPtr> key (*it1 ,*it2);
						  std::pair<NodeSharedPtr,NodeSharedPtr> key_inversed (*it2,*it1);

						  std::map<std::pair<NodeSharedPtr,NodeSharedPtr>, int>::const_iterator iter ;
						  iter= p2_pairs_cooccurance.find(key);

						  if(iter!=p2_pairs_cooccurance.end()){
							  p2_pairs_cooccurance[key]=p2_pairs_cooccurance[key]+1;
						  }
						  else{
						  iter= p2_pairs_cooccurance.find(key_inversed);
						  if(iter!=p2_pairs_cooccurance.end()){
							  p2_pairs_cooccurance[key_inversed]=p2_pairs_cooccurance[key_inversed]+1;
							  }
						  }
					  }
					}
				}
			}

		//Count the agreements between both partitions
		int max_cooccurance_value=0;
		int actual_agreements = 0;
		int max_possible_num_of_agreements =0;
		//Iterate through the keys in the first partitioning map
      	typedef std::map<std::pair<NodeSharedPtr,NodeSharedPtr>, int>::const_iterator MapIterator;
      	for (MapIterator iter = p1_pairs_cooccurance.begin(); iter != p1_pairs_cooccurance.end(); iter++)
      	{
      	   max_possible_num_of_agreements++;
      	  //Get the current key
      	   std::pair<NodeSharedPtr,NodeSharedPtr> key (iter->first.first ,iter->first.second);
      	  //Get the value referred to by the current key
      	  int value_in_p1 = iter->second;
          //Check the value of the same key in the second partitioning map
      	  int value_in_p2 = p2_pairs_cooccurance[key];
      	  if(value_in_p1==value_in_p2){
      		actual_agreements++;
      	  }
      	  //Store the maximum value
      	  if(value_in_p2>max_cooccurance_value | value_in_p1>max_cooccurance_value)
      	  max_cooccurance_value=(value_in_p2 > value_in_p1)?value_in_p2:value_in_p1;

      	}
      	double unadjusted_omega = ((float)actual_agreements/(float)max_possible_num_of_agreements);
      	//std::cout << "unadjusted_omega = " << unadjusted_omega<<std::endl;
      	//std::cout<<"max cooccurance value " << max_cooccurance_value<<std::endl;

      	//calculate the exptected omega index of a null model
      	double omega_null_model=0;
      	long sum_of_multiplications =0;
      	for(int cooccurance_value = 0 ; cooccurance_value<=max_cooccurance_value;cooccurance_value++){
      		//Count how many times the current co-occurance value appeared in each partitioning
      		int happened_in_partitioning1 =0;
      		int happened_in_partitioning2 =0;
      		typedef std::map<std::pair<NodeSharedPtr,NodeSharedPtr>, int>::const_iterator MapIterator;
			for (MapIterator iter = p1_pairs_cooccurance.begin(); iter != p1_pairs_cooccurance.end(); iter++)
			{
				//Get the current key
				std::pair<NodeSharedPtr,NodeSharedPtr> key (iter->first.first ,iter->first.second);
				//If the value equal to the current cooccurance value, then increment happened_in_first_partitioning variable
				if(p1_pairs_cooccurance[key]==cooccurance_value)happened_in_partitioning1++;
			}
			for (MapIterator iter = p2_pairs_cooccurance.begin(); iter != p2_pairs_cooccurance.end(); iter++)
			{
				//Get the current key
				std::pair<NodeSharedPtr,NodeSharedPtr> key (iter->first.first ,iter->first.second);
				//If the value equal to the current cooccurance value, then increment happened_in_first_partitioning variable
				if(p1_pairs_cooccurance[key]==cooccurance_value)happened_in_partitioning2++;
			}
			sum_of_multiplications+=happened_in_partitioning2*happened_in_partitioning2;
      	}
      	//std::cout<<"sum of multiplications " << sum_of_multiplications<<std::endl;
      	omega_null_model= ((float)sum_of_multiplications/(float) pow((float)max_possible_num_of_agreements,(float)2));
      	//std::cout<<"omega_null_model " << omega_null_model<<std::endl;

      	//Calculate the value of omega index
      	double omega_index = (unadjusted_omega-omega_null_model)/(1-omega_null_model);
      	//std::cout<<"omega index " << omega_index<<std::endl;
      	return omega_index;
      }

}
