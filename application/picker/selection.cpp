#include "core.h"

#include "selection.h"
#include "application.h"
#include "view/screen.h"
#include "ecs/components.h"

namespace P3D::Application {

	void Selection::expandBoundingBox(const Vec3& position) {
		if (this->boundingBox.has_value())
			this->boundingBox = this->boundingBox->expanded(position);
		else
			this->boundingBox = BoundingBox(position, position);
	}

	std::size_t Selection::count() const {
		return this->selection.size();
	}

	void Selection::add(const Selection& other, bool recalculateBounds) {
		for (const Engine::Registry64::entity_type& entity : other) 
			this->add(entity, recalculateBounds);
	}

	void Selection::remove(const Selection& other, bool recalculateBounds) {
		for (const Engine::Registry64::entity_type& entity : other)
			this->remove(entity, false);

		if (recalculateBounds) {
			this->boundingBox = std::nullopt;
			for (const Engine::Registry64::entity_type entity : *this) {
				Ref<Comp::Transform> transform = screen.registry.get<Comp::Transform>(entity);
				if (transform.invalid())
					continue;

				expandBoundingBox(castPositionToVec3(transform->getPosition()));
			}
		}
	}

	void Selection::add(const Engine::Registry64::entity_type& entity, bool recalculateBounds) {
		auto[iterator, inserted] = this->selection.insert(entity);

		if (recalculateBounds && inserted) {
			Ref<Comp::Transform> transform = screen.registry.get<Comp::Transform>(entity);
			if (transform.invalid())
				return;

			expandBoundingBox(castPositionToVec3(transform->getPosition()));
		}
	}

	void Selection::remove(const Engine::Registry64::entity_type& entity, bool recalculateBounds) {
		this->selection.erase(entity);

		if (recalculateBounds) {
			this->boundingBox = std::nullopt;
			for (const Engine::Registry64::entity_type entity : *this) {
				Ref<Comp::Transform> transform = screen.registry.get<Comp::Transform>(entity);
				if (transform.invalid())
					continue;

				expandBoundingBox(castPositionToVec3(transform->getPosition()));
			}
		}
	}

	void Selection::clear() {
		this->selection.clear();
		this->boundingBox = std::nullopt;
	}

	BoundingBox Selection::getBoundingBox() const {
		if (this->boundingBox.has_value())
			return this->boundingBox->expanded(0.5);
		else
			return BoundingBox();
	}

	std::set<Engine::Registry64::entity_type>::const_iterator Selection::begin() const {
		return this->selection.begin();
	}
	
	std::set<Engine::Registry64::entity_type>::const_iterator Selection::end() const {
		return this->selection.end();
	}
	
	std::set<Engine::Registry64::entity_type>::iterator Selection::begin() {
		return this->selection.begin();
	}

	std::set<Engine::Registry64::entity_type>::iterator Selection::end() {
		return this->selection.end();
	}


}
