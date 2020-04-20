#pragma once

#include "../engine/ecs/component.h"
#include "../physics/math/globalCFrame.h"

namespace Application {

/*
	A Component representing the position, rotation and scale of the parent entity
*/
class Transform : public Engine::Component {
private:
	/*
		The Cframe of this TransformComp
	*/
	GlobalCFrame cframe;

public:
	DEFINE_COMPONENT(Transform, true);

	/*
		Created a new TransformComp with the given CFrame
	*/
	Transform(const GlobalCFrame& cframe);

	/*
		Return the cframe of this TransformComp
	*/
	GlobalCFrame getCFrame() const;

	/*
		Sets the CFrame of this TransformComp to the given value
	*/
	void setCFrame(const GlobalCFrame& cframe);
};

};