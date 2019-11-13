#include "shapeClass.h"

#include <map>

std::map<int, const ShapeClass*> knownShapeClasses;

int newClassID() {
	if(knownShapeClasses.empty()) return 0;
	for(int i = 0; ; i++) {
		if(knownShapeClasses.find(i) == knownShapeClasses.end()) {
			return i;
		}
	}
}

ShapeClass::ShapeClass(double volume, Vec3 centerOfMass, ScalableInertialMatrix inertia, int classID) : volume(volume), centerOfMass(centerOfMass), inertia(inertia), classID(classID) {
	if(!knownShapeClasses.empty() && knownShapeClasses.find(classID) != knownShapeClasses.end()) throw "ClassID already in use!";
	knownShapeClasses.insert(std::pair<int, const ShapeClass*>(classID, this));
}
ShapeClass::ShapeClass(double volume, Vec3 centerOfMass, ScalableInertialMatrix inertia) : ShapeClass(volume, centerOfMass, inertia, newClassID()) {}


const ShapeClass* ShapeClass::getShapeClassForId(int classID) {
	return knownShapeClasses[classID];
}