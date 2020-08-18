#include "core.h"
#include "transform.h"

#include "../engine/ecs/entity.h"
#include "model.h"

namespace P3D::Application {

Transform::Transform(const GlobalCFrame& cframe) {
	this->cframe = cframe;
}

GlobalCFrame Transform::getCFrame() const {
	Model* model = getEntity()->getComponent<Model>();

	if (model != nullptr)
		return model->getTransform().getCFrame();

	return cframe;
}

void Transform::setCFrame(const GlobalCFrame& cframe) {
	this->cframe = cframe;
}

};