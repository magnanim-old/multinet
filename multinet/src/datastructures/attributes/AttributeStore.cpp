#include "datastructures.h"

namespace mlnet {

AttributeStore::~AttributeStore() {}

AttributeStoreSharedPtr AttributeStore::create() {
	return AttributeStoreSharedPtr(new MainMemoryAttributeStore());
}

MainMemoryAttributeStore::MainMemoryAttributeStore() {}

int MainMemoryAttributeStore::numAttributes() const {
	return attribute_vector.size();
}

const vector<AttributeSharedPtr>& MainMemoryAttributeStore::attributes() const {
	return attribute_vector;
}

AttributeSharedPtr MainMemoryAttributeStore::attribute(int idx) const {
	if (attribute_vector.size()>idx)
		return attribute_vector.at(idx);
	else return AttributeSharedPtr();
}

AttributeSharedPtr MainMemoryAttributeStore::attribute(const string& attribute_name) const {
	if (attribute_ids.count(attribute_name)>0)
		return attribute_vector.at(attribute_ids.at(attribute_name));
	else return AttributeSharedPtr();
}

void MainMemoryAttributeStore::add(const string& attribute_name, attribute_type type) {
	switch (type) {
	case STRING_TYPE:
		if (string_attribute.count(attribute_name)>0) throw DuplicateElementException("Attribute " + attribute_name);
		string_attribute[attribute_name] = hash_map<object_id,string>();
		break;
	case NUMERIC_TYPE:
		if (numeric_attribute.count(attribute_name)>0) throw DuplicateElementException("Attribute " + attribute_name);
		numeric_attribute[attribute_name] = hash_map<object_id,double>();
		break;
	}
	AttributeSharedPtr new_attribute(new Attribute(attribute_name, type));
	attribute_vector.push_back(new_attribute);
	attribute_ids[attribute_name] = attribute_vector.size()-1;
}

void MainMemoryAttributeStore::setNumeric(object_id oid, const string& attribute_name, double val) {
	if (numeric_attribute.count(attribute_name)==0) throw ElementNotFoundException("Numeric attribute " + attribute_name);
	numeric_attribute.at(attribute_name)[oid] = val;
}


void MainMemoryAttributeStore::setString(object_id oid, const string& attribute_name, const string& val) {
	if (string_attribute.count(attribute_name)==0) throw ElementNotFoundException("String attribute " + attribute_name);
	string_attribute.at(attribute_name)[oid] = val;
}


const double& MainMemoryAttributeStore::getNumeric(object_id oid, const string& attribute_name) const {
	if (numeric_attribute.count(attribute_name)==0) throw ElementNotFoundException("Numeric attribute " + attribute_name);
	if (numeric_attribute.at(attribute_name).count(oid)==0) return default_numeric;
	return numeric_attribute.at(attribute_name).at(oid);
}

const string& MainMemoryAttributeStore::getString(object_id oid, const string& attribute_name) const {
	if (string_attribute.count(attribute_name)==0) throw ElementNotFoundException("String attribute " + attribute_name);
	if (string_attribute.at(attribute_name).count(oid)==0) return default_string;
	return string_attribute.at(attribute_name).at(oid);
}

void MainMemoryAttributeStore::reset(object_id oid) {
	for (AttributeSharedPtr attr: attribute_vector) {
		switch (attr->type()) {
		case NUMERIC_TYPE:
			numeric_attribute[attr->name()].erase(oid);
			break;
		case STRING_TYPE:
			string_attribute[attr->name()].erase(oid);
			break;
		}
	}
}

}

