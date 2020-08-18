#include "core.h"
#include "model.h"

#include "extendedPart.h"

namespace P3D::Application {

Model::Model(ExtendedPart* ExtendedPart) {
	this->extendedPart = ExtendedPart;
}

ExtendedPart* Model::getExtendedPart() const {
	return extendedPart;
}

void Model::setExtendedPart(ExtendedPart* extendedPart) {
	this->extendedPart = extendedPart;
}

Transform Model::getTransform() {
	ExtendedPart* extendedPart = getExtendedPart();

	if (extendedPart == nullptr)
		return Transform(GlobalCFrame());

	return Transform(getExtendedPart()->getCFrame());
}

void Model::setTransform(const Transform& transform) {
	getExtendedPart()->setCFrame(transform.getCFrame());
}

};