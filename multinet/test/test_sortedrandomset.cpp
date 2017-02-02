/*
 * Unit testing: sortedrandomset.h
 */

#include "test.h"
#include "utils.h"

using namespace mlnet;

void test_sortedrandomset() {

	test_begin("Sorted Random Set");

	std::cout << "Testing basic functions...";
	sorted_random_set<std::shared_ptr<string> > set;
	std::shared_ptr<string> s1 = std::make_shared<string>("5");
	std::shared_ptr<string> s2 = std::make_shared<string>("8");
	std::shared_ptr<string> s3 = std::make_shared<string>("7");
	std::shared_ptr<string> s4 = std::make_shared<string>("6");
	set.insert(s1);
	set.insert(s2);
	set.insert(s3);
	set.insert(s4);
	set.erase(s1);
	if (!set.contains(s2)) throw FailedUnitTestException("Wrong function: contains");
	if (set.contains(s1)) throw FailedUnitTestException("Wrong function: erase");
	if (set.get_at_index(set.get_index(s2))!=s2) throw FailedUnitTestException("Wrong function: indexes");
	std::cout << "done!" << std::endl;

	std::cout << "Testing random extraction... examples: ";
	std::cout <<  *set.get_at_random() << " " << *set.get_at_random() << " " << *set.get_at_random() << " " << *set.get_at_random() << " " << *set.get_at_random();
	std::cout << " done!" << std::endl;

	test_end("Sorted Random Set");
}
