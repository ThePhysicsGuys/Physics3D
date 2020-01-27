#include "core.h"
#include "transform.h"

#include "../engine/ecs/entity.h"
#include "model.h"

namespace Application {

TransformComp::TransformComp(const GlobalCFrame& cframe) {
	this->cframe = cframe;
}

GlobalCFrame TransformComp::getCFrame() const {
	Model* model = getEntity()->getComponent<Model>();

	if (model != nullptr)
		return model->getTransform().getCFrame();

	return cframe;
}

void TransformComp::setCFrame(const GlobalCFrame& cframe) {
	this->cframe = cframe;
}

};