#include "core.h"

#include "selectionTool.h"

#include "../engine/event/event.h"
#include "../engine/event/mouseEvent.h"
#include "../graphics/resource/textureResource.h"
#include "../util/resource/resourceManager.h"

#include "../../view/screen.h"
#include "../../application.h"
#include "../../shader/shaders.h"
#include "../../input/standardInputHandler.h"
#include "../../ecs/components.h"
#include "../../worlds.h"

#include "../physics/boundstree/filters/rayIntersectsBoundsFilter.h"

#include <optional>

namespace P3D::Graphics {
class TextureResource;
}

namespace P3D::Application {

Ray SelectionTool::ray;
Vec2 SelectionTool::mouse;
Selection SelectionTool::selection;
std::optional<Position> SelectionTool::selectedPoint;
std::optional<Position> SelectionTool::intersectedPoint;

void SelectionTool::onRegister() {
	auto path = "../res/textures/icons/" + getName() + ".png";
	ResourceManager::add<TextureResource>(getName(), path);
}

void SelectionTool::onDeregister() {
	// Remove texture
}

void SelectionTool::onRender() {
	using namespace Graphics;
}

void SelectionTool::onEvent(Engine::Event& event) {
	using namespace Engine;

	EventDispatcher dispatcher(event);
	dispatcher.dispatch<MousePressEvent>(EVENT_BIND(SelectionTool::onMousePress));
}

bool SelectionTool::onMousePress(Engine::MousePressEvent& event) {
	using namespace Engine;

	if (event.getButton() != Mouse::LEFT)
		return false;

	// Multi selection check
	if (!event.getModifiers().isCtrlPressed())
		clear();

	// Single selection
	auto intersectedEntity = getIntersectedEntity();

	if (intersectedEntity.has_value())
		toggle(intersectedEntity->first);
	
	return false;
}

void SelectionTool::clear() {
	selection.clear();
}

void SelectionTool::toggle(const Engine::Registry64::entity_type& entity) {
	selection.toggle(entity);
}

void SelectionTool::select(const Engine::Registry64::entity_type& entity) {
	selection.add(entity);
}

std::optional<std::pair<Engine::Registry64::entity_type, Position>> SelectionTool::getIntersectedEntity() {
	Engine::Registry64::entity_type intersectedEntity = 0;
	double closestIntersectionDistance = std::numeric_limits<double>::max();
	auto view = screen.registry.view<Comp::Hitbox, Comp::Transform>();
	for (auto entity : view) {
		Ref<Comp::Hitbox> hitbox = view.get<Comp::Hitbox>(entity);
		Ref<Comp::Transform> transform = view.get<Comp::Transform>(entity);
		screen.world->syncReadOnlyOperation([&] () {
			std::optional<double> distance = intersect(transform->getCFrame(), hitbox);
			if (distance.has_value() && distance < closestIntersectionDistance) {
				closestIntersectionDistance = *distance;
				intersectedEntity = entity;
			}
		});
	}

	if (intersectedEntity == 0)
		return std::nullopt;

	Position intersection = ray.origin + ray.direction * closestIntersectionDistance;
	
	return std::make_pair(intersectedEntity, intersection);
}

std::optional<double> SelectionTool::intersect(const GlobalCFrame& cframe, Ref<Comp::Hitbox> hitbox) {
	Shape shape = hitbox->getShape();
	Vec3 relativePosition = cframe.getPosition() - ray.origin;
	double maxRadius = shape.getMaxRadius();
	if (pointToLineDistanceSquared(ray.direction, relativePosition) > maxRadius * maxRadius)
		return std::nullopt;

	RayIntersectBoundsFilter filter(ray);
	if (hitbox->isPartAttached())
		if (!filter(*hitbox->getPart()))
			return std::nullopt;

	double distance = shape.getIntersectionDistance(cframe.globalToLocal(ray.origin), cframe.relativeToLocal(ray.direction));

	if (distance == 0.0 || distance == std::numeric_limits<double>::max())
		return std::nullopt;

	return distance;
}

std::optional<double> SelectionTool::intersect(const GlobalCFrame& cframe, const Shape& shape) {
	Vec3 relativePosition = cframe.getPosition() - ray.origin;
	double maxRadius = shape.getMaxRadius();
	if (pointToLineDistanceSquared(ray.direction, relativePosition) > maxRadius * maxRadius)
		return std::nullopt;

	double distance = shape.getIntersectionDistance(cframe.globalToLocal(ray.origin), cframe.relativeToLocal(ray.direction));
	if (distance == 0.0 || distance == std::numeric_limits<double>::max())
		return std::nullopt;

	return distance;
}

std::optional<double> SelectionTool::intersect(const GlobalCFrame& cframe, const VisualShape& shape) {
	Vec3 relativePosition = cframe.getPosition() - ray.origin;
	//double maxRadius = shape.getMaxRadius();
	//if (pointToLineDistanceSquared(ray.direction, relativePosition) > maxRadius * maxRadius)
	//	return std::nullopt;
	
	double distance = shape.getIntersectionDistance(cframe.globalToLocal(ray.origin), cframe.relativeToLocal(ray.direction));
	if (distance == 0.0 || distance == std::numeric_limits<double>::max())
		return std::nullopt;

	return distance;
}

};
