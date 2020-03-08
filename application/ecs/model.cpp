#include "core.h"
#include "model.h"

#include "../physics/part.h"

namespace Application {

Model::Model(Part* part) {
	this->extendedPart = part;
}

Part* Model::getExtendedPart() const {
	return extendedPart;
}

void Model::setExtendedPart(Part* extendedPart) {
	this->extendedPart = extendedPart;
}

TransformComponent Model::getTransform() {
	Part* extendedPart = getExtendedPart();

	if (extendedPart == nullptr)
		return TransformComponent(GlobalCFrame());

	return TransformComponent(getExtendedPart()->getCFrame());
}

void Model::setTransform(const TransformComponent& transform) {
	getExtendedPart()->setCFrame(transform.getCFrame());
}

};