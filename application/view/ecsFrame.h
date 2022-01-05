#pragma once

#include "../engine/ecs/registry.h"

namespace P3D {
	class Part;
	class SoftLink;
	struct HardPhysicalConnection;

	namespace Application::Comp {
		struct Collider;
	}
}

namespace P3D::Application {
	struct ECSFrame {
	private:
		static void renderEntity(Engine::Registry64& registry, const Engine::Registry64::entity_type& entity);
		static void renderHardConstraints(Engine::Registry64& registry, const Engine::Registry64::entity_type& entity, const Comp::Collider& collider, const std::vector<HardPhysicalConnection*>& hardConstraints);
		static void renderSoftLinks(Engine::Registry64& registry, const Engine::Registry64::entity_type& entity, const Comp::Collider& collider, const std::vector<SoftLink*>& softLinks);
		static void renderAttachments(Engine::Registry64& registry, const Engine::Registry64::entity_type& entity, const Comp::Collider& collider, const std::vector<Part*>& attachments);

	public:
		static void onInit(Engine::Registry64& registry);
		static void onRender(Engine::Registry64& registry);

	};


}
