#pragma once

#include <Physics3D/externalforces/externalForce.h>

namespace P3D::Application {

class PlayerController : public ExternalForce {
	virtual void apply(WorldPrototype* world) override;
};

};
