
#ifndef WALKTRAP_H_
#define WALKTRAP_H_



/**
 * 
 */
#include <iostream>
#include <datastructures.h>
#include <community.h>

using namespace std;

namespace mlnet {

    class Neighbor {
    public:
      int community1;   // the two adjacent communities
      int community2;   // community1 < community2

      float delta_sigma;	// the delta sigma between the two communities
      float weight;		// the total weight of the edges between the two communities
      bool exact;		// true if delta_sigma is exact, false if it is only a lower bound
      
      Neighbor* next_community1;	    // pointers of two double
      Neighbor* previous_community1;    // chained lists containing
      Neighbor* next_community2;	    // all the neighbors of
      Neighbor* previous_community2;    // each communities.

      int heap_index;	// 
      
      Neighbor();
    };

    class Neighbor_heap {  
    private:
      int size;
      int max_size;

      Neighbor** H;   // the heap that contains a pointer to each Neighbor object stored

      void move_up(int index);
      void move_down(int index);
      
    public:
      void add(Neighbor* N);	    // add a new distance
      void update(Neighbor* N);	    // update a distance 
      void remove(Neighbor* N);	    // remove a distance
      Neighbor* get_first();	    // get the first item
      long memory();
      bool is_empty();
      
      Neighbor_heap(int max_size);
      ~Neighbor_heap();
    };

    class Min_delta_sigma_heap {
    private:
      int size;
      int max_size;

      int* H;   // the heap that contains the number of each community
      int* I;   // the index of each community in the heap (-1 = not stored)

      void move_up(int index);
      void move_down(int index);

    public:
      int get_max_community();			    // return the community with the maximal delta_sigma
      void remove_community(int community);		    // remove a community;
      void update(int community);			    // update (or insert if necessary) the community
      long memory();				    // the memory used in Bytes.
      bool is_empty();

      float* delta_sigma;				     // the delta_sigma of the stored communities
      
      Min_delta_sigma_heap(int max_size);
      ~Min_delta_sigma_heap();
    };

    class WalktrapEdge {			// code an edge of a given vertex
    public:
      int neighbor;			// the number of the neighbor vertex
      float weight;			// the weight of the edge
    };
    bool operator<(const WalktrapEdge& E1, const WalktrapEdge& E2);


    class WalktrapVertex {
    public:
      WalktrapEdge* edges;			// the edges of the vertex
      int degree;			// number of neighbors
      float total_weight;		// the total weight of the vertex

      WalktrapVertex();			// creates empty vertex
      ~WalktrapVertex();			// destructor
    };

    class WalktrapGraph {
    public:
      int nb_vertices;		// number of vertices
      int nb_edges;			// number of edges
      float total_weight;		// total weight of the edges
      WalktrapVertex* vertices;		// array of the vertices

      long memory();			// the total memory used in Bytes
      WalktrapGraph();			// create an empty graph
      ~WalktrapGraph();			// destructor
      char** index;			// to keep the real name of the vertices
      bool load_index(char* input_file);
    };

    istream& operator>>(istream& in, WalktrapGraph& G);	// get a graph from a stream

    void fromLayerToGraph (const MLNetworkSharedPtr& mlnet, const LayerSharedPtr& layer, WalktrapGraph& wTrapGraph);

    class WalkTrapCommunities;

    class WalkTrapProbabilities {
    public:
      static float* tmp_vector1;	// 
      static float* tmp_vector2;	// 
      static int* id;	    // 
      static int* vertices1;    //
      static int* vertices2;    //  
      static int current_id;    // 

      static WalkTrapCommunities* C;				    // pointer to all the communities
      static int length;					              // length of the random walks

      
      int size;						    // number of probabilities stored
      int* vertices;					    // the vertices corresponding to the stored probabilities, 0 if all the probabilities are stored
      float* P;						    // the probabilities
      
      long memory();					    // the memory (in Bytes) used by the object
      double compute_distance(const WalkTrapProbabilities* P2) const;   // compute the squared distance r^2 between this probability vector and P2
      WalkTrapProbabilities(int community);				    // compute the probability vector of a community
      WalkTrapProbabilities(int community1, int community2);	    // merge the probability vectors of two communities in a new one
    							    // the two communities must have their probability vectors stored
    							    
      ~WalkTrapProbabilities();					    // destructor
    };

    class WalkTrapCommunity {
    public:
      
      Neighbor* first_neighbor;	// first item of the list of adjacent communities
      Neighbor* last_neighbor;	// last item of the list of adjacent communities
      
      int this_community;		// number of this community
      int first_member;		// number of the first vertex of the community
      int last_member;		// number of the last vertex of the community
      int size;			// number of members of the community
      
      WalkTrapProbabilities* P;		// the probability vector, 0 if not stored.  


      float sigma;			// sigma(C) of the community
      float internal_weight;	// sum of the weight of the internal edges
      float total_weight;		// sum of the weight of all the edges of the community (an edge between two communities is a half-edge for each community)
        
      int sub_communities[2];	// the two sub sommunities, -1 if no sub communities;
      int sub_community_of;		// number of the community in which this community has been merged
    				// 0 if the community is active
    				// -1 if the community is not used
      
      void merge(WalkTrapCommunity &C1, WalkTrapCommunity &C2);	// create a new community by merging C1 an C2
      void add_neighbor(Neighbor* N);
      void remove_neighbor(Neighbor* N);
      float min_delta_sigma();			// compute the minimal delta sigma among all the neighbors of this community


      
      WalkTrapCommunity();			// create an empty community
      ~WalkTrapCommunity();			// destructor
    };

    class WalkTrapCommunities {

    private:
      bool silent;		// whether the progression is displayed
      int details;		// between 0 and 3, how much details are printed
      long max_memory;	// size in Byte of maximal memory usage, -1 for no limit
      
    public:
      
      long memory_used;				    // in bytes
      Min_delta_sigma_heap* min_delta_sigma;    	    // the min delta_sigma of the community with a saved probability vector (for memory management)
      
      WalktrapGraph* G;		    // the graph
      int* members;		    // the members of each community represented as a chained list.
    			    // a community points to the first_member the array which contains 
    			    // the next member (-1 = end of the community)
      Neighbor_heap* H;	    // the distances between adjacent communities.


      WalkTrapCommunity* communities;	// array of the communities
      
      int nb_communities;		// number of valid communities 
      int nb_active_communities;	// number of active communities
      
      float find_best_modularity(int community, bool* max_modularity);
      void print_best_modularity_partition();
      void print_best_modularity_partition(int community, bool* max_modularity);

      
      WalkTrapCommunities(WalktrapGraph* G, int random_walks_length = 3, bool silent = false, int details = 1, long max_memory = -1);    // Constructor
      ~WalkTrapCommunities();					// Destructor


      void merge_communities(Neighbor* N);			// create a community by merging two existing communities
      double merge_nearest_communities();
      
      double compute_delta_sigma(int c1, int c2);		// compute delta_sigma(c1,c2) 

      void remove_neighbor(Neighbor* N);
      void add_neighbor(Neighbor* N);
      void update_neighbor(Neighbor* N, float new_delta_sigma);

      void manage_memory();
      
      void print_state();
      void print_partition(int nb_remaining_commities);	// print the partition for a given number of communities
      void print_community(int c);				// print a community  
      
      CommunitiesSharedPtr  toMlnetCommunities(const LayerSharedPtr& layer); //this function transform from the community structure found in Pascal Pons code to the community structure in our code


    };

}
#endif /* WALKTRAP_H_ */