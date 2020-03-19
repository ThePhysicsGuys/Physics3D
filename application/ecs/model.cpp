#include "core.h"
#include "model.h"

#include "extendedPart.h"

namespace Application {

Model::Model(ExtendedPart* ExtendedPart) {
	this->extendedPart = ExtendedPart;
}

ExtendedPart* Model::getExtendedPart() const {
	return extendedPart;
}

void Model::setExtendedPart(ExtendedPart* extendedPart) {
	this->extendedPart = extendedPart;
}

TransformComponent Model::getTransform() {
	ExtendedPart* extendedPart = getExtendedPart();

	if (extendedPart == nullptr)
		return TransformComponent(GlobalCFrame());

	return TransformComponent(getExtendedPart()->getCFrame());
}

void Model::setTransform(const TransformComponent& transform) {
	getExtendedPart()->setCFrame(transform.getCFrame());
}

};