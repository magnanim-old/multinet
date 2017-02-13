/*
 * Generic data structures for community detection
 */

#include "utils.h"
#include <iostream>

#include "community.h"

namespace mlnet {

/*
community::community() {}

sorted_set<ActorSharedPtr> community::actors() {
	std::set<ActorSharedPtr> actors;
	for (CliqueSharedPtr clique: cliques) {
		for (ActorSharedPtr actor: clique->actors)
			actors.insert(actor);
	}
	return actors;
}

std::string community::to_string() {
	std::string res = "C" + std::to_string(id) + ": ";
	hash_set<ActorSharedPtr> actors;
	for (CliqueSharedPtr clique: cliques) {
		for (ActorSharedPtr actor: clique->actors)
			actors.insert(actor);
	}
	for (ActorSharedPtr actor: actors)
		res += actor->name + " ";
	res += "( ";
	for (LayerSharedPtr layer: layers) {
		res += layer->name + " ";
	}
	res += ")";
	return res;
}

int community::size() const {
	hash_set<ActorSharedPtr> actors;
	for (CliqueSharedPtr clique: cliques) {
		for (ActorSharedPtr actor: clique->actors)
			actors.insert(actor);
	}
	return actors.size();
}
*/

}
