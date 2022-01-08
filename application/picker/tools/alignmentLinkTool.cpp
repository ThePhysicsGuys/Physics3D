#include "core.h"
#include "alignmentLinkTool.h"

#include "application.h"
#include "selectionTool.h"
#include "graphics/resource/textureResource.h"
#include "util/resource/resourceManager.h"
#include "view/screen.h"

namespace P3D::Application {

void AlignmentLinkTool::onRegister() {
	auto path = "../res/textures/icons/" + getName() + ".png";
	ResourceManager::add<Graphics::TextureResource>(getName(), path);
}

void AlignmentLinkTool::onDeregister() {
	// Remove texture
}

void AlignmentLinkTool::onSelect() {
	linkSelection();
}

void AlignmentLinkTool::linkSelection() {
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
		AttachedPart partA { cframe, parentCollider->part };
		AttachedPart partB { CFrame(), childCollider->part };
		AlignmentLink* link = new AlignmentLink(partA, partB);

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
