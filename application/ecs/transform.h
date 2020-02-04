#pragma once

#include "../engine/ecs/component.h"
#include "../physics/math/globalCFrame.h"

namespace Application {

/*
	A Component representing the position, rotation and scale of the parent entity
*/
class TransformComponent : public Engine::Component {
private:
	/*
		The Cframe of this TransformComp
	*/
	GlobalCFrame cframe;

public:
	DEFINE_COMPONENT(TransformComponent, true);

	/*
		Created a new TransformComp with the given CFrame
	*/
	TransformComponent(const GlobalCFrame& cframe);

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