#include "core.h"
#include "springLinkTool.h"

#include "application.h"
#include "selectionTool.h"
#include "graphics/resource/textureResource.h"
#include "Physics3D/softlinks/springLink.h"
#include "util/resource/resourceManager.h"
#include "view/screen.h"

namespace P3D::Application {

void SpringLinkTool::onRegister() {
	auto path = "../res/textures/icons/" + getName() + ".png";
	ResourceManager::add<Graphics::TextureResource>(getName(), path);
}

void SpringLinkTool::onDeregister() {
	// Remove texture
}

void SpringLinkTool::onSelect() {
	linkSelection();
}

void SpringLinkTool::linkSelection() {
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
		AttachedPart part1 { cframe, parentCollider->part };
		AttachedPart part2 { CFrame(), childCollider->part };
		SpringLink* link = new SpringLink(part1, part2, 5.0, 1.0);

		screen.worldMutex->lock();
		try {
			world.addLink(link);
		} catch (std::invalid_argument& error) {
			Log::debug(error.what());
		}
		screen.worldMutex->unlock();
	}
}

}
