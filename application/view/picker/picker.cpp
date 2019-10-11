#include "core.h"

#include "picker.h"

#include "../engine/math/mathUtil.h"

#include "event/event.h"
#include "event/mouseEvent.h"
#include "input/mouse.h"

#include "ray.h"

#include "../screen.h"
#include "../material.h"
#include "../renderUtils.h"
#include "../mesh/primitive.h"
#include "../mesh/indexedMesh.h"
#include "../shaderProgram.h"

#include "../gui/gui.h"

#include "../../application.h"
#include "../../io/import.h"

#include "../engine/physical.h"
#include "../engine/sharedLockGuard.h"
#include "../engine/geometry/shape.h"
#include "../engine/misc/filters/rayIntersectsBoundsFilter.h"
#include "view/debug/visualDebug.h"

namespace Picker {

	EditTools editTools = EditTools();

	// Init
	void onInit() {
		editTools.onInit();
	}

	void onEvent(Event& event) {
		EventDispatcher dispatcher(event);

		dispatcher.dispatch<MousePressEvent>(onMousePress);
		dispatcher.dispatch<MouseReleaseEvent>(onMouseRelease);
		dispatcher.dispatch<MouseDragEvent>(onMouseDrag);
	}

	// Render
	void onRender(Screen& screen) {
		Shaders::maskShader.updateProjection(screen.camera.viewMatrix, screen.camera.projectionMatrix);

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
	float intersect(const Ray& ray, const Shape& shape, const GlobalCFrame& cframe) {
		return shape.getIntersectionDistance(cframe.globalToLocal(ray.start), cframe.relativeToLocal(ray.direction));
	}

	// Calculates the closest intersection of the given ray with the physicals in the world
	void intersectPhysicals(Screen& screen, const Ray& ray) {

		ExtendedPart* closestIntersectedPart = nullptr;
		Position closestIntersectedPoint = Position();
		float closestIntersectDistance = INFINITY;
		
		graphicsMeasure.mark(GraphicsProcess::WAIT_FOR_LOCK);
		screen.world->syncReadOnlyOperation([&screen, &closestIntersectDistance, &closestIntersectedPart, &closestIntersectedPoint, &ray]() {
			graphicsMeasure.mark(GraphicsProcess::PICKER);
			for (ExtendedPart& part : screen.world->iterPartsFiltered(RayIntersectBoundsFilter(ray))) {
				if (&part == screen.camera.attachment) continue;
				Vec3 relPos = part.getPosition() - ray.start;
				if (pointToLineDistanceSquared(ray.direction, relPos) > part.maxRadius * part.maxRadius)
					continue;

				float distance = intersect(ray, part.hitbox, part.getCFrame());

				if (distance < closestIntersectDistance && distance > 0) {
					closestIntersectDistance = distance;
					closestIntersectedPart = &part;
				}
			}
		});

		if (closestIntersectDistance == INFINITY) {
			closestIntersectedPart = nullptr;
			closestIntersectedPoint = ray.start;
		} else {
			closestIntersectedPoint = ray.start + ray.direction * closestIntersectDistance;
		}

		// Update intersected part
		screen.intersectedPart = closestIntersectedPart;
		screen.intersectedPoint = closestIntersectedPoint;

		// Call callback
		(*screen.eventHandler.partRayIntersectHandler) (screen, closestIntersectedPart, closestIntersectedPoint);
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
	bool onMousePress(MousePressEvent& event) {
		// Check if users pressed on tool
		if (Mouse::LEFT != event.getButton())
			return false;

		if (screen.selectedPart && editTools.intersectedEditDirection != EditTools::EditDirection::NONE) {
			editTools.onMousePress(screen);
		} else { // Keep current part selected as long as tool is being used
			// Update selected part
			screen.selectedPart = screen.intersectedPart;
			screen.selectedPoint = screen.intersectedPoint;

			// Update intersected point if a physical has been intersected and move physical
			if (screen.intersectedPart) {
				screen.world->asyncModification([]() {
					screen.world->localSelectedPoint = screen.selectedPart->getCFrame().globalToLocal(screen.intersectedPoint);
					moveGrabbedPhysicalLateral(screen);
				});
			}
		}

		return true;
	}

	// Mouse release behaviour
	bool onMouseRelease(MouseReleaseEvent& event) {
		if (Mouse::LEFT != event.getButton())
			return false;

		// Reset selectedpart
		screen.world->selectedPart = nullptr;

		// Reset selected tool
		editTools.onMouseRelease(screen);

		return true;
	}

	
	// Mouse drag behaviour
	bool onMouseDrag(MouseDragEvent& event) {
		if (!event.isLeftDragging())
			return false;

		if (editTools.selectedEditDirection != EditTools::EditDirection::NONE) {
			editTools.onMouseDrag(screen);
		} else {
			if (screen.selectedPart) {
				screen.world->asyncModification([]() {
					moveGrabbedPhysicalLateral(screen);
				});
			}
		}

		return true;
	}

	void moveGrabbedPhysicalLateral(Screen& screen) {
		if (screen.selectedPart == nullptr) return;
		
		Mat3 cameraFrame = (screen.camera.cframe.rotation).transpose();
		Vec3 cameraDirection = cameraFrame * Vec3(0, 0, 1);

		double distance = Vec3(screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (screen.ray * cameraDirection);
		Position planeIntersection = screen.camera.cframe.position + screen.ray * distance;

		if (isPaused()) {
			Vec3 translation = planeIntersection - screen.selectedPoint;
			screen.selectedPoint += translation;
			screen.selectedPart->parent->translate(translation);
		} else {
			screen.world->selectedPart = screen.selectedPart;
			screen.world->magnetPoint = planeIntersection;
		}
	}

	void moveGrabbedPhysicalTransversal(Screen& screen, double dz) {
		if (screen.selectedPart == nullptr) return;
		
		Mat3 cameraFrame = screen.camera.cframe.rotation.transpose();
		Vec3 cameraDirection = cameraFrame * Vec3(0, 0, 1);
		Vec3 cameraYDirection = normalize(Vec3(cameraDirection.x, 0, cameraDirection.z));

		double distance = Vec3(screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (screen.ray * cameraDirection);
		Position planeIntersection = screen.camera.cframe.position + screen.ray * distance;

		Vec3 translation = -cameraYDirection * dz;
		if (isPaused()) {
			screen.selectedPoint += translation;
			screen.selectedPart->parent->translate(translation);
		} else {
			screen.world->selectedPart = screen.selectedPart;
			screen.world->magnetPoint += translation;
		}
	}
}

