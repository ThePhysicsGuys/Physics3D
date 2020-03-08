#pragma once

#include "../engine/ecs/component.h"
#include "transform.h"

struct Part;

namespace Application {

/*
	A Component representing a model which is being physicsed by the physics engine, all the transform of the entity with this component will be taken from the model
*/
class Model : public Engine::Component {
private:
	/*
		The extendedPart of this model
	*/
	Part* extendedPart;

public:
	DEFINE_COMPONENT(Model, true);

	/*
		Creates a model with the given extendedpart
	*/
	Model(Part* part);

	/*
		Returns the extendedPart of this model
	*/
	Part* getExtendedPart() const;

	/*
		Sets the extendedPart of this model to the given value
	*/
	void setExtendedPart(Part* extendedPart);

	/*
		Returns the transform of this model's extendedPart
	*/
	TransformComponent getTransform();

	/*
		Sets the transform of this model's ExtendedPart to the given value
	*/
	void setTransform(const TransformComponent& transform);
};

};