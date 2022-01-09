#pragma once

#include "../engine/ecs/registry.h"
#include "../application/ecs/components.h"

#include <optional>

namespace P3D::Application {

	struct Selection {
	private:
		// A bounding box in the cframe of the first entity in the selection
		std::optional<BoundingBox> boundingBox;
		std::vector<Engine::Registry64::entity_type> selection;

		void expandSelection(const Engine::Registry64::entity_type& entity);
		
	public:
		[[nodiscard]] bool empty() const;
		[[nodiscard]] std::size_t size() const;
		[[nodiscard]] bool contains(const Engine::Registry64::entity_type& entity) const;
		[[nodiscard]] bool isMultiSelection() const;
		[[nodiscard]] Engine::Registry64::entity_type& operator[](int index);
		
		void add(const Selection& other, bool recalculateBounds = true);
		void add(const Engine::Registry64::entity_type& entity, bool recalculateBounds = true);
		void remove(const Selection& other, bool recalculateBounds = true);
		void remove(const Engine::Registry64::entity_type& entity, bool recalculateBounds = true);
		void toggle(const Engine::Registry64::entity_type& entity, bool recalculateBounds = true);
		void clear();

		void translate(const Vec3& translation);
		void rotate(const Vec3& normal, double angle);
		void scale(const Vec3& scale);
		
		void recalculateSelection();
		
		[[nodiscard]] std::optional<GlobalCFrame> getCFrame() const;
		[[nodiscard]] std::optional<Shape> getHitbox() const;

		[[nodiscard]] std::vector<Engine::Registry64::entity_type>::const_iterator begin() const;
		[[nodiscard]] std::vector<Engine::Registry64::entity_type>::const_iterator end() const;
		[[nodiscard]] std::vector<Engine::Registry64::entity_type>::iterator begin();
		[[nodiscard]] std::vector<Engine::Registry64::entity_type>::iterator end();
		[[nodiscard]] std::optional<Engine::Registry64::entity_type> first() const;
		[[nodiscard]] std::optional<Engine::Registry64::entity_type> last() const;
	};
	
}
