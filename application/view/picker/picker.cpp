#include "picker.h"

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
#include "../engine/math/mat3.h"
#include "../engine/math/vec4.h"
#include "../engine/math/mathUtil.h"
#include "../engine/sharedLockGuard.h"
#include "../engine/geometry/shape.h"

#include "../util/log.h"

#include "../engine/math/tempUnsafeCasts.h"

namespace Picker {

	EditTools editTools = EditTools();

	// Init
	void init() {
		editTools.init();
	}

	// Render
	void render(Screen& screen) {
		if (screen.selectedPart) {
			editTools.render(screen);
		}
	}

	// Intersections
	// Intersection distance of the given ray with the given shape, transformed with the given cframe. 
	float intersect(const Ray& ray, const VisualShape& shape, const GlobalCFrame& cframe) {
		return shape.getIntersectionDistance(cframe.globalToLocal(ray.start), cframe.relativeToLocal(ray.direction));
	}

	// Calculates the closest intersection of the given ray with the physicals in the world
	void intersectPhysicals(Screen& screen, const Ray& ray) {
		SharedLockGuard guard(screen.world->lock);

		ExtendedPart* closestIntersectedPart = nullptr;
		Position closestIntersectedPoint = Position();
		float closestIntersectDistance = INFINITY;

		for (ExtendedPart& part : *screen.world) {
			Vec3 relPos = TEMP_CAST_VEC_TO_POSITION(part.cframe.position) - ray.start;
			if (ray.direction.pointToLineDistanceSquared(relPos) > part.maxRadius * part.maxRadius)
				continue;

			float distance = intersect(ray, part.visualShape, TEMP_CAST_CFRAME_TO_GLOBALCFRAME(part.cframe));

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
		screen.intersectedPart = closestIntersectedPart;
		screen.intersectedPoint = closestIntersectedPoint;

		// Call callback
		(*screen.eventHandler.partRayIntersectHandler) (screen, closestIntersectedPart, closestIntersectedPoint);
	}

	// Update
	// Calculates the mouse ray and the tool & physical intersections
	void update(Screen& screen, Vec2 mousePosition) {
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
	void press(Screen& screen) {
		// Check if users pressed on tool
		
		if (editTools.intersectedEditDirection != EditTools::EditDirection::NONE) {
			editTools.press(screen);
		} else { // Keep current part selected as long as tool is being used
			// Update selected part
			screen.selectedPart = screen.intersectedPart;
			screen.selectedPoint = screen.intersectedPoint;

			// Update intersected point if a physical has been intersected and move physical
			if (screen.intersectedPart) {
				screen.world->localSelectedPoint = TEMP_CAST_CFRAME_TO_GLOBALCFRAME(screen.selectedPart->cframe).globalToLocal(screen.intersectedPoint);
				moveGrabbedPhysicalLateral(screen);
			}
		}
	}

	// Mouse release behaviour
	void release(Screen& screen) {
		// Reset selectedpart
		screen.world->selectedPart = nullptr;

		// Reset selected tool
		editTools.release(screen);
	}

	
	// Mouse drag behaviour
	void drag(Screen& screen) {
		if (editTools.selectedEditDirection != EditTools::EditDirection::NONE) {
			editTools.drag(screen);
		} else {
			if (screen.selectedPart) {
				moveGrabbedPhysicalLateral(screen);
			}
		}
	}

	void moveGrabbedPhysicalLateral(Screen& screen) {
		if (screen.selectedPart == nullptr) return;

		Mat3 cameraFrame = (screen.camera.cframe.rotation).transpose();
		Vec3 cameraDirection = cameraFrame * Vec3(0, 0, 1);

		double distance = Vec3(screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (screen.ray * cameraDirection);
		Position planeIntersection = screen.camera.cframe.position + distance * screen.ray;

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
		Vec3 cameraYDirection = Vec3(cameraDirection.x, 0, cameraDirection.z).normalize();

		double distance = Vec3(screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (screen.ray * cameraDirection);
		Position planeIntersection = screen.camera.cframe.position + distance * screen.ray;

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

