#pragma once

#include "extendedPart.h"
#include <Physics3D/math/position.h>
#include <Physics3D/threading/synchronizedWorld.h>

namespace P3D::Application {

class PlayerWorld : public SynchronizedWorld<ExtendedPart> {
public:
	PlayerWorld(double deltaT);

	void onPartAdded(ExtendedPart* part) override;
	void onPartRemoved(ExtendedPart* part) override;
};

};