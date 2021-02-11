#include "core.h"

#include "picker.h"

#include "../physics/math/mathUtil.h"

#include "../engine/event/event.h"
#include "../engine/event/mouseEvent.h"
#include "../engine/input/mouse.h"

#include "ray.h"

#include "view/screen.h"

#include "application.h"
#include "shader/shaders.h"
#include "ecs/components.h"

#include "worlds.h"
#include "../../graphics/debug/visualDebug.h"
#include "../physics/sharedLockGuard.h"
#include "../physics/geometry/shape.h"
#include "../physics/misc/filters/rayIntersectsBoundsFilter.h"

namespace P3D::Application {

namespace Picker {

EditTools editTools = EditTools();

// Init
void onInit() {
	editTools.onInit();
}

void onEvent(Engine::Event& event) {
	using namespace Engine;

	EventDispatcher dispatcher(event);

	dispatcher.dispatch<MousePressEvent>(onMousePress);
	dispatcher.dispatch<MouseReleaseEvent>(onMouseRelease);
	dispatcher.dispatch<MouseDragEvent>(onMouseDrag);
}

// Render
void onRender(Screen& screen) {
	if (screen.selectedPart) {
		editTools.onRender(screen);
	}
}

// Close
void onClose() {
	editTools.onClose();
}

// Intersections
// Intersection distance of the given ray with the given shape, transformed with the given cframe. 
double intersect(const Ray& ray, const Shape& shape, const GlobalCFrame& cframe) {
	return shape.getIntersectionDistance(cframe.globalToLocal(ray.start), cframe.relativeToLocal(ray.direction));
}

// Calculates the closest intersection of the given ray with the physicals in the world
void intersectPhysicalss(Screen& screen, const Ray& ray) {

	ExtendedPart* closestIntersectedPart = nullptr;
	Position closestIntersectedPoint = Position();
	float closestIntersectDistance = INFINITY;

	for (ExtendedPart& part : screen.world->iterPartsFiltered(RayIntersectBoundsFilter(ray))) {
		if (&part == screen.camera.attachment) 
			continue;
		
		Vec3 relativePosition = part.getPosition() - ray.start;
		if (pointToLineDistanceSquared(ray.direction, relativePosition) > part.maxRadius * part.maxRadius)
			continue;

		double distance = intersect(ray, part.hitbox, part.getCFrame());

		if (distance < closestIntersectDistance && distance > 0) {
			closestIntersectDistance = distance;
			closestIntersectedPart = &part;
		}
	}

	if (closestIntersectDistance == INFINITY) {
		closestIntersectedPart = nullptr;
		closestIntersectedPoint = ray.start;
	} else {
		closestIntersectedPoint = ray.start + ray.direction * closestIntersectDistance;
	}

	// Update intersected part
	// REPLACE
	if (closestIntersectedPart)
		screen.intersectedEntity = closestIntersectedPart->entity;
	else
		screen.intersectedEntity = 0;
	screen.intersectedPart = closestIntersectedPart;
	screen.intersectedPoint = closestIntersectedPoint;

	// Call callback
	(*screen.eventHandler.partRayIntersectHandler) (screen, closestIntersectedPart, closestIntersectedPoint);
}

void intersectPhysicals(Screen& screen, const Ray& ray) {

	Engine::Registry64::entity_type closestIntersectedEntity = 0;
	Position closestIntersectedPoint = Position();
	double closestIntersectDistance = std::numeric_limits<double>::max();

	RayIntersectBoundsFilter filter(ray);
	
	auto view = screen.registry.view<Comp::Hitbox>();
	for (auto entity : view) {
		Ref<Comp::Hitbox> hitbox = view.get<Comp::Hitbox>(entity);
		if (hitbox->isPartAttached())
			if (!filter(*hitbox->getPart()))
				continue;

		Ref<Comp::Transform> transform = screen.registry.get<Comp::Transform>(entity);
		if (transform.invalid())
			continue;

		Shape shape = hitbox->getShape();
		Vec3 relativePosition = transform->getPosition() - ray.start;
		if (pointToLineDistanceSquared(ray.direction, relativePosition) > shape.getMaxRadius() * shape.getMaxRadius())
			continue;

		double distance = intersect(ray, shape, transform->getCFrame());
		
		if (distance < closestIntersectDistance && distance > 0) {
			closestIntersectDistance = distance;
			closestIntersectedEntity = entity;
		}
	}

	if (closestIntersectDistance == std::numeric_limits<double>::max()) {
		closestIntersectedEntity = 0;
		closestIntersectedPoint = ray.start;
	} else {
		closestIntersectedPoint = ray.start + ray.direction * closestIntersectDistance;
	}

	// Update intersection variables
	screen.intersectedEntity = closestIntersectedEntity;
	screen.intersectedPoint = closestIntersectedPoint;
	auto collider = screen.registry.get<Comp::Collider>(screen.intersectedEntity);
	screen.intersectedPart = collider.valid() ? collider->part : nullptr;

	// Call callback
	(*screen.eventHandler.partRayIntersectHandler) (screen, screen.intersectedPart, screen.intersectedPoint);
}

// Update
// Calculates the mouse ray and the tool & physical intersections
void onUpdate(Screen& screen, Vec2 mousePosition) {
	graphicsMeasure.mark(GraphicsProcess::PICKER);
	// Calculate ray
	Ray ray = getMouseRay(screen, mousePosition);

	// Update screen ray
	screen.ray = ray.direction;

	// Intersect edit tools
	if (screen.selectedPart) {
		editTools.intersect(screen, ray);
		// No physical intersection check needed if tool is being intersected
		if (editTools.intersectedEditDirection != EditTools::EditDirection::NONE)
			return;
	}

	// Intersect physcials
	intersectPhysicals(screen, ray);
}


// Events
// Mouse press behaviour
bool onMousePress(Engine::MousePressEvent& event) {
	// Check if users pressed on tool
	if (Engine::Mouse::LEFT != event.getButton())
		return false;

	if (screen.selectedPart && editTools.intersectedEditDirection != EditTools::EditDirection::NONE) {
		editTools.onMousePress(screen);
	} else { // Keep current part selected as long as tool is being used
		// Update selected part
		// REPLACE
		screen.selectedEntity = screen.intersectedEntity;
		screen.selectedPart = screen.intersectedPart;
		screen.selectedPoint = screen.intersectedPoint;
		screen.world->selectedPart = screen.intersectedPart;

		// Update intersected point if a physical has been intersected and move physical
		// REPLACE if (screen.intersectedEntity)
		if (screen.intersectedPart) {
			screen.world->asyncModification([] () {
				screen.world->localSelectedPoint = screen.selectedPart->getCFrame().globalToLocal(screen.intersectedPoint);
				moveGrabbedPhysicalLateral(screen);
			});
		}
	}

	return true;
}

// Mouse release behaviour
bool onMouseRelease(Engine::MouseReleaseEvent& event) {
	if (Engine::Mouse::LEFT != event.getButton())
		return false;

	// Reset selectedpart
	screen.world->selectedPart = nullptr;

	// Reset selected tool
	editTools.onMouseRelease(screen);

	return true;
}


// Mouse drag behaviour
bool onMouseDrag(Engine::MouseDragEvent& event) {
	if (!event.isLeftDragging())
		return false;

	if (editTools.selectedEditDirection != EditTools::EditDirection::NONE) {
		editTools.onMouseDrag(screen);
	} else {
		if (screen.selectedPart) {
			screen.world->asyncModification([] () {
				moveGrabbedPhysicalLateral(screen);
				});
		}
	}

	return true;
}

void moveGrabbedPhysicalLateral(Screen& screen) {
	if (screen.world->selectedPart == nullptr) return;

	Vec3 cameraDirection = screen.camera.cframe.rotation * Vec3(0, 0, 1);

	double distance = Vec3(screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (screen.ray * cameraDirection);
	Position planeIntersection = screen.camera.cframe.position + screen.ray * distance;

	if (isPaused()) {
		Vec3 translation = planeIntersection - screen.selectedPoint;
		screen.selectedPoint += translation;
		screen.selectedPart->translate(translation);
	} else {
		screen.world->selectedPart = screen.selectedPart;
		screen.world->magnetPoint = planeIntersection;
	}
}

void moveGrabbedPhysicalTransversal(Screen& screen, double dz) {
	if (screen.world->selectedPart == nullptr) return;

	Vec3 cameraDirection = screen.camera.cframe.rotation * Vec3(0, 0, 1);
	Vec3 cameraYDirection = normalize(Vec3(cameraDirection.x, 0, cameraDirection.z));

	double distance = Vec3(screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (screen.ray * cameraDirection);
	Position planeIntersection = screen.camera.cframe.position + screen.ray * distance;

	Vec3 translation = -cameraYDirection * dz;
	if (isPaused()) {
		screen.selectedPoint += translation;
		screen.selectedPart->translate(translation);
	} else {
		screen.world->selectedPart = screen.selectedPart;
		screen.world->magnetPoint += translation;
	}
}
}

};