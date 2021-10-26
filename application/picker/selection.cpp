#include "core.h"

#include "selection.h"
#include "application.h"
#include "view/screen.h"
#include "ecs/components.h"

namespace P3D::Application {

	void Selection::recalculateSelection() {
		this->boundingBox = std::nullopt;
		for (auto entity : this->selection)
			expandSelection(entity);
	}

	void Selection::expandSelection(const Engine::Registry64::entity_type& entity) {
		IRef<Comp::Transform> transform = screen.registry.get<Comp::Transform>(entity);
		if (transform.invalid())
			return;

		IRef<Comp::Hitbox> hitbox = screen.registry.get<Comp::Hitbox>(entity);
		
		if (selection.empty() || !this->boundingBox.has_value()) {
			if (hitbox.valid())
				this->boundingBox = hitbox->getShape().getBounds();
			else
				this->boundingBox = BoundingBox(0.2, 0.2, .2);
		} else {
			IRef<Comp::Transform> referenceTransform = screen.registry.get<Comp::Transform>(selection[0]);
			GlobalCFrame referenceFrame = referenceTransform->getCFrame();
			CFrame relativeFrame = referenceFrame.globalToLocal(transform->getCFrame());

			if (hitbox.valid()) {
				BoundingBox rotatedBounds = hitbox->getShape().getBounds(relativeFrame.getRotation());
				this->boundingBox = this->boundingBox->expanded(relativeFrame.getPosition() + rotatedBounds.min).expanded(relativeFrame.getPosition() + rotatedBounds.max);
			} else {
				this->boundingBox = this->boundingBox->expanded(Shape(&CubeClass::instance, 0.2, 0.2, 0.2).getBounds(relativeFrame.getRotation()));
			}
		}
	}

	bool Selection::empty() const {
		return this->selection.empty();
	}

	std::size_t Selection::size() const {
		return this->selection.size();
	}

	bool Selection::contains(const Engine::Registry64::entity_type& entity) const {
		for(const Engine::Registry64::entity_type& found : selection)
			if(found == entity)
				return true;

		return false;
	}

	bool Selection::isMultiSelection() const {
		return size() > 1;
	}

	Engine::Registry64::entity_type& Selection::operator[](int index) {
		return this->selection[index];
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
			for (const Engine::Registry64::entity_type entity : *this)
				expandSelection(entity);
		}
	}

	void Selection::add(const Engine::Registry64::entity_type& entity, bool recalculateBounds) {
		auto iterator = std::find(this->selection.begin(), this->selection.end(), entity);

		if (iterator != this->selection.end())
			return;
		
		this->selection.push_back(entity);

		if (recalculateBounds) 
			expandSelection(entity);
		
	}

	void Selection::remove(const Engine::Registry64::entity_type& entity, bool recalculateBounds) {
		auto iterator = std::find(this->selection.begin(), this->selection.end(), entity);
		if (iterator == this->selection.end())
			return;
		
		this->selection.erase(iterator);

		if (recalculateBounds) {
			this->boundingBox = std::nullopt;
			for (const Engine::Registry64::entity_type entity : *this) 
				expandSelection(entity);
		}
	}

	void Selection::toggle(const Engine::Registry64::entity_type& entity, bool recalculateBounds) {
		auto iterator = std::find(this->selection.begin(), this->selection.end(), entity);
		
		if (iterator == selection.end())
			add(entity, recalculateBounds);
		else
			remove(entity, recalculateBounds);
	}

	void Selection::clear() {
		this->selection.clear();
		this->boundingBox = std::nullopt;
	}

	void Selection::translate(const Vec3& translation) {
		for (auto entity : this->selection) {
			IRef<Comp::Transform> transform = screen.registry.get<Comp::Transform>(entity);
			if (transform.valid())
				transform->translate(translation);
		}
	}

	void Selection::rotate(const Vec3& normal, double angle) {
		std::optional<GlobalCFrame> reference = getCFrame();
		if (!reference.has_value())
			return;
		
		Rotation rotation = Rotation::fromRotationVec(angle * normal);
		for (auto entity : this->selection) {
			IRef<Comp::Transform> transform = screen.registry.get<Comp::Transform>(entity);
			
			if (transform.valid()) {
				transform->rotate(rotation);
				Vec3 delta = transform->getPosition() - reference->getPosition();
				Vec3 translation = rotation * delta - delta;
				transform->translate(translation);
			}
		}
	}

	void Selection::scale(const Vec3& scale) {
		std::optional<GlobalCFrame> reference = getCFrame();
		if (!reference.has_value())
			return;

		for (auto entity : this->selection) {
			IRef<Comp::Transform> transform = screen.registry.get<Comp::Transform>(entity);
			Vec3 delta = transform->getPosition() - reference->getPosition();
			Vec3 translation = elementWiseMul(delta, scale - Vec3(1.0, 1.0, 1.0));
			transform->translate(translation);
			transform->scale(scale.x, scale.y, scale.z);
		}

		recalculateSelection();
	}

	std::optional<Shape> Selection::getHitbox() const {
		if (this->selection.empty())
			return std::nullopt;

		if (this->size() == 1) {
			IRef<Comp::Hitbox> hitbox = screen.registry.get<Comp::Hitbox>(this->selection[0]);
			if (hitbox.valid())
				return hitbox->getShape();
		}

		return Shape(&CubeClass::instance, boundingBox->getWidth(), boundingBox->getHeight(), boundingBox->getDepth());
	}

	std::optional<GlobalCFrame> Selection::getCFrame() const {
		if (this->selection.empty())
			return std::nullopt;

		IRef<Comp::Transform> transform = screen.registry.get<Comp::Transform>(this->selection[0]);
		return GlobalCFrame(transform->getCFrame().localToGlobal(this->boundingBox->getCenter()), transform->getRotation());
	}
	
	std::vector<Engine::Registry64::entity_type>::const_iterator Selection::begin() const {
		return this->selection.begin();
	}
	
	std::vector<Engine::Registry64::entity_type>::const_iterator Selection::end() const {
		return this->selection.end();
	}
	
	std::vector<Engine::Registry64::entity_type>::iterator Selection::begin() {
		return this->selection.begin();
	}
	
	std::vector<Engine::Registry64::entity_type>::iterator Selection::end() {
		return this->selection.end();
	}


}
