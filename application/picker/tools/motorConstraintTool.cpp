#include "core.h"
#include "motorConstraintTool.h"

#include "application.h"
#include "selectionTool.h"
#include "graphics/resource/textureResource.h"
#include "Physics3D/hardconstraints/constraintTemplates.h"
#include "Physics3D/hardconstraints/fixedConstraint.h"
#include "Physics3D/hardconstraints/controller/constController.h"
#include "util/resource/resourceManager.h"
#include "view/screen.h"

namespace P3D::Application {

void MotorConstraintTool::onRegister() {
	auto path = "../res/textures/icons/" + getName() + ".png";
	ResourceManager::add<Graphics::TextureResource>(getName(), path);
}

void MotorConstraintTool::onDeregister() {
	// Remove texture
}

void MotorConstraintTool::onSelect() {
	constrainSelection();
}

void MotorConstraintTool::constrainSelection() {
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
		MotorConstraintTemplate<ConstController>* constraint = new MotorConstraintTemplate<ConstController>(1.0);

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
