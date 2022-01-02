#include "core.h"
#include "elasticLinkTool.h"

#include "application.h"
#include "selectionTool.h"
#include "graphics/resource/textureResource.h"
#include "Physics3D/softlinks/elasticLink.h"
#include "util/resource/resourceManager.h"
#include "view/screen.h"

namespace P3D::Application {

void ElasticLinkTool::onRegister() {
	auto path = "../res/textures/icons/" + getName() + ".png";
	ResourceManager::add<Graphics::TextureResource>(getName(), path);
}

void ElasticLinkTool::onDeregister() {
	// Remove texture
}

void ElasticLinkTool::onSelect() {
	linkSelection();
}

void ElasticLinkTool::linkSelection() {
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
		SoftLink::AttachedPart part1(cframe, parentCollider->part);
		SoftLink::AttachedPart part2(CFrame(), childCollider->part);
		ElasticLink* link = new ElasticLink(part1, part2, 5.0, 1.0);

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
