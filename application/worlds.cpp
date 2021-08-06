#include "core.h"

#include "worlds.h"

#include "application.h"
#include "view/screen.h"
#include "input/standardInputHandler.h"
#include <Physics3D/misc/debug.h>
#include "../engine/options/keyboardOptions.h"
#include <Physics3D/externalforces/magnetForce.h>
#include "input/playerController.h"

#include "ecs/components.h"

namespace P3D::Application {


PlayerWorld::PlayerWorld(double deltaT) : SynchronizedWorld<ExtendedPart>(deltaT) {
	this->addExternalForce(new PlayerController());
}

void PlayerWorld::onPartAdded(ExtendedPart* part) {
	screen.registry.add<Comp::Collider>(part->entity, part);
}

void PlayerWorld::onPartRemoved(ExtendedPart* part) {
	screen.registry.remove<Comp::Collider>(part->entity);
	screen.registry.get<Comp::Transform>(part->entity)->cframe = Comp::Transform::ScaledCFrame { part->getCFrame(), part->hitbox.scale };
}

};