#pragma once

#include <map>

class ShapeClass;

class PartSerializationInformation {
public:
	std::map<const ShapeClass*, unsigned int> shapeClassRegistry;
};
class PartDeSerializationInformation {
public:
	std::map<unsigned int, const ShapeClass*> shapeClassRegistry;
};
