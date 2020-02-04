#include "core.h"
#include "transform.h"

#include "../engine/ecs/entity.h"
#include "model.h"

namespace Application {

TransformComponent::TransformComponent(const GlobalCFrame& cframe) {
	this->cframe = cframe;
}

GlobalCFrame TransformComponent::getCFrame() const {
	Model* model = getEntity()->getComponent<Model>();

	if (model != nullptr)
		return model->getTransform().getCFrame();

	return cframe;
}

void TransformComponent::setCFrame(const GlobalCFrame& cframe) {
	this->cframe = cframe;
}

};