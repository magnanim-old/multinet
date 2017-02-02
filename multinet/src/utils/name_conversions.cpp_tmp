/*
 * name_conversions.cpp
 *
 * Created on: Jul 29, 2014
 * Author: matteomagnani
 * Version: 0.0.1
 */

#include "utils.h"


std::set<network_id> network_names_to_ids(const MultiplexNetwork& mnet, const std::vector<std::string>& names) {
	std::set<network_id> res;
	for (std::string name: names) {
		res.insert(mnet.getNetworkId(name));
	}
	return res;
}

std::set<identity> identity_names_to_ids(const MultiplexNetwork& mnet, const std::vector<std::string>& names) {
	std::set<identity> res;
	for (std::string name: names) {
		res.insert(mnet.getGlobalIdentity(name));
	}
	return res;
}
