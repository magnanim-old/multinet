/*
 * Unit testing: sortedrandommap.h
 */

#include "test.h"
#include "utils.h"

using namespace mlnet;

void test_sortedrandommap() {

	test_begin("Sorted Random Map");

	std::cout << "Testing basic functions...";
	sorted_random_map<int,std::shared_ptr<string> > map;
	std::shared_ptr<string> s1 = std::make_shared<string>("5");
	std::shared_ptr<string> s2 = std::make_shared<string>("8");
	std::shared_ptr<string> s3 = std::make_shared<string>("7");
	std::shared_ptr<string> s4 = std::make_shared<string>("6");
	map.insert(0,s1);
	map.insert(13,s2);
	map.insert(2,s3);
	map.insert(1,s4);
	map.erase(1);
	if (!map.contains(0)) throw FailedUnitTestException("Wrong function: contains");
	if (map.contains(1)) throw FailedUnitTestException("Wrong function: erase");
	if (map.get(2)!=s3) throw FailedUnitTestException("Wrong function: get");
	if (map.get(1)) throw FailedUnitTestException("Wrong function: get (value not present)");
	if (map.get_at_index(map.get_index(13))!=s2) throw FailedUnitTestException("Wrong function: indexes");
	std::cout << "done!" << std::endl;

	std::cout << "Testing random extraction... examples: ";
	std::cout <<  *map.get_at_random() << " " << *map.get_at_random() << " " << *map.get_at_random() << " " << *map.get_at_random() << " " << *map.get_at_random();
	std::cout << " done!" << std::endl;

	test_end("Sorted Random Map");
}
