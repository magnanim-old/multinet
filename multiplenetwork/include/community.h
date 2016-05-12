#ifndef MULTIPLENETWORK_COMMUNITY_H_
#define MULTIPLENETWORK_COMMUNITY_H_

#include "datastructures.h"
#include <unordered_set>
#include <utility>

namespace mlnet {

class community {
public:
	community(long id, simple_set<CliqueSharedPtr> cliques, simple_set<LayerSharedPtr> layers);
	community();
	std::set<ActorSharedPtr> actors();
	int size() const;
	bool operator==(const community& comp) const;
	bool operator!=(const community& comp) const;
	bool operator<(const community& comp) const;
	bool operator>(const community& comp) const;
	std::string to_string();

	long id;
	std::set<CliqueSharedPtr> cliques;
	std::set<LayerSharedPtr> layers;
};

typedef std::shared_ptr<community> CommunitySharedPtr;

simple_set<CommunitySharedPtr> ml_cpm(MLNetworkSharedPtr mnet, int k, int m1, int m2);

hashtable<ActorSharedPtr,simple_set<LayerSharedPtr> > get_mlneighbors(MLNetworkSharedPtr mnet, ActorSharedPtr actor);

simple_set<LayerSharedPtr> neighboring_layers(MLNetworkSharedPtr mnet, ActorSharedPtr actor1, ActorSharedPtr actor2);

simple_set<CliqueSharedPtr> find_cliques(MLNetworkSharedPtr mnet, int k, int m);

void find_cliques(MLNetworkSharedPtr mnet, CliqueSharedPtr A, hashtable<ActorSharedPtr,simple_set<LayerSharedPtr> > B, simple_set<CliqueSharedPtr>& C, int k, int m);

simple_set<CliqueSharedPtr> find_max_cliques(MLNetworkSharedPtr mnet, simple_set<LayerSharedPtr> layers, int k, int m);

simple_set<CliqueSharedPtr> find_max_cliques(MLNetworkSharedPtr mnet, int k, int m);

void find_max_cliques(MLNetworkSharedPtr mnet, CliqueSharedPtr& A, hashtable<ActorSharedPtr,simple_set<LayerSharedPtr> >& B, hashtable<ActorSharedPtr,simple_set<LayerSharedPtr> >& C, simple_set<CliqueSharedPtr>& result, int k, int m);

std::map<CliqueSharedPtr,simple_set<CliqueSharedPtr> > build_adjacency_graph(const simple_set<CliqueSharedPtr>& C, int m);

std::map<CliqueSharedPtr,simple_set<CliqueSharedPtr> > build_max_adjacency_graph(const simple_set<CliqueSharedPtr>& C, int k, int m);

void find_communities(const std::map<CliqueSharedPtr,simple_set<CliqueSharedPtr> >& adjacency, CommunitySharedPtr A, const simple_set<CliqueSharedPtr>& B, simple_set<CliqueSharedPtr> U, simple_set<CommunitySharedPtr>& C, int m);

simple_set<CommunitySharedPtr> find_communities(MLNetworkSharedPtr mnet, const std::map<CliqueSharedPtr,simple_set<CliqueSharedPtr> >& adjacency, int m);

simple_set<CommunitySharedPtr> find_max_communities(MLNetworkSharedPtr mnet, const std::map<CliqueSharedPtr,simple_set<CliqueSharedPtr> >& adjacency, int m);

void find_max_communities2(const std::map<CliqueSharedPtr,simple_set<CliqueSharedPtr> >& adjacency, CommunitySharedPtr A, hashtable<CliqueSharedPtr,simple_set<LayerSharedPtr> > B, hashtable<CliqueSharedPtr,simple_set<LayerSharedPtr> > D, hashtable<CliqueSharedPtr,simple_set<LayerSharedPtr> > U, simple_set<CommunitySharedPtr>& C, int m);

void find_max_communities(const std::map<CliqueSharedPtr,simple_set<CliqueSharedPtr> >& adjacency, CommunitySharedPtr& A, hashtable<CliqueSharedPtr,simple_set<LayerSharedPtr> >& B, hashtable<CliqueSharedPtr,simple_set<LayerSharedPtr> >& C, hashtable<CliqueSharedPtr,simple_set<LayerSharedPtr> >& D, simple_set<CommunitySharedPtr>& result, int m);

}
#endif /* MULTIPLENETWORK_COMMUNITY_H_ */

