

#include "utils.h"
#include <iostream>
#include <ctime>
#include <set>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "community/singleLayer/singleLayer.h"
#include "community/singleLayer/walktrap.h"
#include "datastructures.h"
#include "community.h"

using namespace std;

namespace mlnet {


    

    CommunitiesSharedPtr  SingleLayer::walkTrap(const MLNetworkSharedPtr& mlnet, const LayerSharedPtr& layer)
    {
    	  int length = 4;
		  int details = 2;
		  long max_memory = -1;
		  bool silent = false;

    
           
  		  WalktrapGraph* wTrapGraph = new WalktrapGraph;
  		  fromLayerToGraph (mlnet, layer, *wTrapGraph);

		  WalkTrapCommunities walkTrapComs(wTrapGraph, length, silent, details, max_memory);  

		  while(!walkTrapComs.H->is_empty()) {
		    walkTrapComs.merge_nearest_communities();
		  }
           
		  //Create a pointer to the returend communities. 
		  CommunitiesSharedPtr coms = walkTrapComs.toMlnetCommunities(layer);
		  return coms;
    }

}



	
