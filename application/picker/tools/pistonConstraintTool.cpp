#include "core.h"
#include "pistonConstraintTool.h"

#include "application.h"
#include "selectionTool.h"
#include "graphics/resource/textureResource.h"
#include "Physics3D/hardconstraints/fixedConstraint.h"
#include "Physics3D/hardconstraints/sinusoidalPistonConstraint.h"
#include "util/resource/resourceManager.h"
#include "view/screen.h"

namespace P3D::Application {

void PistonConstraintTool::onRegister() {
	auto path = "../res/textures/icons/" + getName() + ".png";
	ResourceManager::add<Graphics::TextureResource>(getName(), path);
}

void PistonConstraintTool::onDeregister() {
	// Remove texture
}

void PistonConstraintTool::onSelect() {
	constrainSelection();
}

void PistonConstraintTool::constrainSelection() {
	if (SelectionTool::selection.size() < 2)
		return;

	IRef<Comp::Collider> parentCollider = screen.registry.get<Comp::Collider>(SelectionTool::selection[0]);
	if (parentCollider.invalid())
		return;

	for (Engine::Registry64::entity_type& entity : SelectionTool::selection) {
		IRef<Comp::Collider> childCollider = screen.registry.get<Comp::Collider>(entity);
		if (childCollider.invalid())
			continue;

		if (childCollider->part == parentCollider->part)
			continue;

		CFrame cframe = parentCollider->part->getCFrame().globalToLocal(childCollider->part->getCFrame());
		SinusoidalPistonConstraint* constraint = new SinusoidalPistonConstraint(2, 5, 1);

		screen.worldMutex->lock();
		try {
			parentCollider->part->attach(childCollider->part, constraint, cframe, CFrame());
		} catch (std::invalid_argument& error) {
			Log::debug(error.what());
		}
		screen.worldMutex->unlock();
	}
}

}
