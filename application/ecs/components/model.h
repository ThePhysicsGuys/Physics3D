#pragma once

#include "component.h"
#include "transform.h"

struct ExtendedPart;

/*
	A Component representing a model which is being physicsed by the physics engine, all the transform of the entity with this component will be taken from the model
*/
class Model : public Component {
private:
	/*
		The extendedPart of this model
	*/
	ExtendedPart* extendedPart;

public:
	/*
		Returns the extendedPart of this model
	*/
	ExtendedPart* getExtendedPart() const;

	/*
		Sets the extendedPart of this model to the given value
	*/
	void setExtendedPart(ExtendedPart* extendedPart);

	/*
		Returns the transform of this model's extendedPart
	*/
	TransformComp getTransform();

	/*
		Sets the transform of this model's ExtendedPart to the given value
	*/
	void setTransform(const TransformComp& transform);

	/*
		Returns whether an Entity can contain multiple instances of this Component
	*/
	virtual bool isUnique() const override;

};

