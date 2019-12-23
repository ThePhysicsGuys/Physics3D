#include "core.h"
#include "transform.h"

#include "../entity.h"
#include "model.h"

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

bool TransformComp::isUnique() const {
	return true;
}
