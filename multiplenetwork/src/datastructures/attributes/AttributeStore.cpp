#include "datastructures.h"
#include "exceptions.h"

namespace mlnet {

int AttributeStore::numAttributes() const {
	return attribute_vector.size();
}

const std::vector<AttributeSharedPtr>& AttributeStore::attributes() const {
	return attribute_vector;
}

AttributeSharedPtr AttributeStore::attribute(int idx) const {
	if (attribute_vector.size()>idx)
		return attribute_vector.at(idx);
	else return NULL;
}

AttributeSharedPtr AttributeStore::attribute(const std::string& attribute_name) const {
	if (attribute_ids.count(attribute_name)>0)
		return attribute_vector.at(attribute_ids.at(attribute_name));
	else return NULL;
}

void AttributeStore::add(const std::string& attribute_name, attribute_type type) {
	switch (type) {
	case STRING_TYPE:
		if (string_attribute.count(attribute_name)>0) throw DuplicateElementException("Attribute " + attribute_name);
		string_attribute[attribute_name] = std::map<object_id,std::string>();
		break;
	case NUMERIC_TYPE:
		if (numeric_attribute.count(attribute_name)>0) throw DuplicateElementException("Attribute " + attribute_name);
		numeric_attribute[attribute_name] = std::map<object_id,double>();
		break;
	}
	AttributeSharedPtr new_attribute(new Attribute(attribute_name, type));
	attribute_vector.push_back(new_attribute);
	attribute_ids[attribute_name] = attribute_vector.size()-1;
}

void AttributeStore::setNumeric(const object_id& oid, const std::string& attribute_name, double val) {
	if (numeric_attribute.count(attribute_name)==0) throw ElementNotFoundException("Numeric attribute " + attribute_name);
	numeric_attribute.at(attribute_name)[oid] = val;
}


void AttributeStore::setString(const object_id& oid, const std::string& attribute_name, const std::string& val) {
	if (string_attribute.count(attribute_name)==0) throw ElementNotFoundException("String attribute " + attribute_name);
	string_attribute.at(attribute_name)[oid] = val;
}


const double& AttributeStore::getNumeric(const object_id& oid, const std::string& attribute_name) const {
	if (numeric_attribute.count(attribute_name)==0) throw ElementNotFoundException("Numeric attribute " + attribute_name);
	if (numeric_attribute.at(attribute_name).count(oid)==0) return default_numeric;
	return numeric_attribute.at(attribute_name).at(oid);
}

const std::string& AttributeStore::getString(const object_id& oid, const std::string& attribute_name) const {
	if (string_attribute.count(attribute_name)==0) throw ElementNotFoundException("String attribute " + attribute_name);
	if (string_attribute.at(attribute_name).count(oid)==0) return default_string;
	return string_attribute.at(attribute_name).at(oid);
}

void AttributeStore::remove(const object_id& oid) {
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

