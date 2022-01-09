#include "core.h"
#include "attachmentTool.h"

#include "application.h"
#include "selectionTool.h"
#include "graphics/resource/textureResource.h"
#include "util/resource/resourceManager.h"
#include "view/screen.h"

namespace P3D::Application {

void AttachmentTool::onRegister() {
	auto path = "../res/textures/icons/" + getName() + ".png";
	ResourceManager::add<Graphics::TextureResource>(getName(), path);
}

void AttachmentTool::onDeregister() {
	// Remove texture
}

void AttachmentTool::onSelect() {
	attachSelection();
}

void AttachmentTool::attachSelection() {
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

		screen.worldMutex->lock();
		try {
			parentCollider->part->attach(childCollider->part, cframe);	
		} catch (std::invalid_argument& error) {
			Log::debug(error.what());
		}
		screen.worldMutex->unlock();
	}
}

}
