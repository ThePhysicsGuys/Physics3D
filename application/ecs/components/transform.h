#pragma once

#include "component.h"

#include "../physics/math/globalCFrame.h"

namespace Application {

/*
	A Component representing the position, rotation and scale of the parent entity
*/
class TransformComp : public Component {
private:
	/*
		The Cframe of this TransformComp
	*/
	GlobalCFrame cframe;

public:
	/*
		Created a new TransformComp with the given CFrame
	*/
	TransformComp(const GlobalCFrame& cframe);

	/*
		Return the cframe of this TransformComp
	*/
	GlobalCFrame getCFrame() const;

	/*
		Sets the CFrame of this TransformComp to the given value
	*/
	void setCFrame(const GlobalCFrame& cframe);

	/*
		Returns whether an Entity can contain multiple instances of this Component
	*/
	virtual bool isUnique() const override;
};

};