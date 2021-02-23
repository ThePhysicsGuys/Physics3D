#pragma once

#include "../engine/ecs/registry.h"
#include "../physics/math/bounds.h"
#include <optional>
#include <set>

namespace P3D::Application {

	struct Selection {
	private:
		std::optional<BoundingBox> boundingBox;
		std::set<Engine::Registry64::entity_type> selection;

		void expandBoundingBox(const Vec3& position);
		
	public:
		[[nodiscard]] std::size_t count() const;
		
		void add(const Selection& other, bool recalculateBounds = true);
		void add(const Engine::Registry64::entity_type& entity, bool recalculateBounds = true);
		void remove(const Selection& other, bool recalculateBounds = true);
		void remove(const Engine::Registry64::entity_type& entity, bool recalculateBounds = true);
		void clear();

		[[nodiscard]] BoundingBox getBoundingBox() const;

		[[nodiscard]] std::set<Engine::Registry64::entity_type>::const_iterator begin() const;
		[[nodiscard]] std::set<Engine::Registry64::entity_type>::const_iterator end() const;
		[[nodiscard]] std::set<Engine::Registry64::entity_type>::iterator begin();
		[[nodiscard]] std::set<Engine::Registry64::entity_type>::iterator end();
	};
	
}
