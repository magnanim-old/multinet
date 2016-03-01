#ifndef MULTIPLENETWORK_COMMUNITY_H_
#define MULTIPLENETWORK_COMMUNITY_H_

#include "datastructures.h"
#include <unordered_set>
#include <utility>

namespace mlnet {

class clique {
public:
	clique(long id, std::unordered_set<ActorSharedPtr> actors, std::unordered_set<LayerSharedPtr> layers);
	clique();
	bool operator==(const clique& comp) const;
	bool operator!=(const clique& comp) const;
	bool operator<(const clique& comp) const;
	bool operator>(const clique& comp) const;
	std::string to_string();

	long id;
	std::unordered_set<ActorSharedPtr> actors;
	std::unordered_set<LayerSharedPtr> layers;
};

typedef std::shared_ptr<clique> CliqueSharedPtr;

class community {
public:
	community(long id, std::unordered_set<CliqueSharedPtr> cliques, std::unordered_set<LayerSharedPtr> layers);
	community();
	std::set<ActorSharedPtr> actors();
	int size() const;
	bool operator==(const community& comp) const;
	bool operator!=(const community& comp) const;
	bool operator<(const community& comp) const;
	bool operator>(const community& comp) const;
	std::string to_string();

	long id;
	std::unordered_set<CliqueSharedPtr> cliques;
	std::unordered_set<LayerSharedPtr> layers;
};

typedef std::shared_ptr<community> CommunitySharedPtr;

std::set<CommunitySharedPtr> ml_cpm(MLNetworkSharedPtr mnet, int k, int m1, int m2);

hash<ActorSharedPtr,std::unordered_set<LayerSharedPtr> > get_mlneighbors(MLNetworkSharedPtr mnet, ActorSharedPtr actor);

std::unordered_set<LayerSharedPtr> neighboring_layers(MLNetworkSharedPtr mnet, ActorSharedPtr actor1, ActorSharedPtr actor2);

std::set<CliqueSharedPtr> find_cliques(MLNetworkSharedPtr mnet, int k, int m);

void find_cliques(MLNetworkSharedPtr mnet, CliqueSharedPtr A, hash<ActorSharedPtr,std::unordered_set<LayerSharedPtr> > B, std::set<CliqueSharedPtr>& C, int k, int m);

std::set<CliqueSharedPtr> find_max_cliques(MLNetworkSharedPtr mnet, int k, int m);

void find_max_cliques(MLNetworkSharedPtr mnet, CliqueSharedPtr& A, hash<ActorSharedPtr,std::unordered_set<LayerSharedPtr> >& B, hash<ActorSharedPtr,std::unordered_set<LayerSharedPtr> >& C, std::set<CliqueSharedPtr>& result, int k, int m);

std::map<CliqueSharedPtr,std::set<CliqueSharedPtr> > build_adjacency_graph(const std::set<CliqueSharedPtr>& C, int m);

std::map<CliqueSharedPtr,std::set<CliqueSharedPtr> > build_max_adjacency_graph(const std::set<CliqueSharedPtr>& C, int k, int m);

void find_communities(const std::map<CliqueSharedPtr,std::set<CliqueSharedPtr> >& adjacency, CommunitySharedPtr A, const std::unordered_set<CliqueSharedPtr>& B, std::set<CliqueSharedPtr> U, std::set<CommunitySharedPtr>& C, int m);

std::set<CommunitySharedPtr> find_communities(MLNetworkSharedPtr mnet, const std::map<CliqueSharedPtr,std::set<CliqueSharedPtr> >& adjacency, int m);

std::set<CommunitySharedPtr> find_max_communities(MLNetworkSharedPtr mnet, const std::map<CliqueSharedPtr,std::set<CliqueSharedPtr> >& adjacency, int m);

void find_max_communities2(const std::map<CliqueSharedPtr,std::set<CliqueSharedPtr> >& adjacency, CommunitySharedPtr A, hash<CliqueSharedPtr,std::unordered_set<LayerSharedPtr> > B, hash<CliqueSharedPtr,std::unordered_set<LayerSharedPtr> > D, hash<CliqueSharedPtr,std::unordered_set<LayerSharedPtr> > U, std::set<CommunitySharedPtr>& C, int m);

void find_max_communities(const std::map<CliqueSharedPtr,std::set<CliqueSharedPtr> >& adjacency, CommunitySharedPtr& A, hash<CliqueSharedPtr,std::unordered_set<LayerSharedPtr> >& B, hash<CliqueSharedPtr,std::unordered_set<LayerSharedPtr> >& C, hash<CliqueSharedPtr,std::unordered_set<LayerSharedPtr> >& D, std::set<CommunitySharedPtr>& result, int m);

}
#endif /* MULTIPLENETWORK_COMMUNITY_H_ */

