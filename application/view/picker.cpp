#include "picker.h"

#include "screen.h"
#include "../../engine/physical.h"
#include "../util/log.h"
#include "../engine/math/mathUtil.h"
#include "../application.h"
#include "../engine/sharedLockGuard.h"

Vec2f getNormalizedDeviceSpacePosition(Vec2f viewportSpacePosition, Vec2f screenSize) {
	float x = 2 * viewportSpacePosition.x / screenSize.x - 1;
	float y = 2 * viewportSpacePosition.y / screenSize.y - 1;
	return Vec2f(x, -y);
}

Vec3f calcRay(Screen& screen, Vec2f mousePosition) {
	Vec2f normalizedDeviceSpacePosition = getNormalizedDeviceSpacePosition(mousePosition, screen.dimension);
	Vec4f clipSpacePosition = Vec4f(normalizedDeviceSpacePosition.x, normalizedDeviceSpacePosition.y, -1.0f, 1.0f);
	Vec4f eyeCoordinates = screen.camera.invertedProjectionMatrix * clipSpacePosition;
	eyeCoordinates = Vec4f(eyeCoordinates.x, eyeCoordinates.y, -1.0f, 0.0f);
	Vec4f rayWorld = screen.camera.invertedViewMatrix * eyeCoordinates;
	Vec3 rayDirection = Vec3(rayWorld.x, rayWorld.y, rayWorld.z).normalize();

	return rayDirection;
}

void updateIntersectedPhysical(Screen& screen, Vec2 mousePosition) {
	SharedLockGuard guard(screen.world->lock);
	
	ExtendedPart* closestIntersectedPart = nullptr;
	Vec3f closestIntersectedPoint = Vec3();
	float closestIntersectDistance = INFINITY;

	Vec3 ray = calcRay(screen, mousePosition);
	Vec3 rayStart = screen.camera.cframe.position;
	screen.ray = ray;
	for(ExtendedPart& part : *screen.world){
		Vec3 relPos = part.cframe.position - rayStart;
		if (ray.pointToLineDistanceSquared(relPos) > part.maxRadius * part.maxRadius)
			continue;

		Vec3f* buffer = new Vec3f[part.hitbox.vertexCount];
		Shape transformed = part.hitbox.localToGlobal(CFramef(part.cframe), buffer);

		float distance = transformed.getIntersectionDistance(Vec3f(rayStart), Vec3f(ray));

		if (distance < closestIntersectDistance && distance > 0) {
			closestIntersectDistance = distance;
			closestIntersectedPart = &part;
		}
		delete[] buffer;
	}

	if (closestIntersectDistance == INFINITY) {
		closestIntersectedPart = nullptr;
		closestIntersectedPoint = screen.camera.cframe.position;
	} else {
		closestIntersectedPoint = screen.camera.cframe.position + screen.ray * closestIntersectDistance;
	}
	
	(*screen.eventHandler.partRayIntersectHandler) (screen, closestIntersectedPart, closestIntersectedPoint);
}

void moveGrabbedPhysicalLateral(Screen& screen) {
	if (screen.selectedPart == nullptr) return;

	Mat3 cameraFrame = (screen.camera.cframe.rotation).transpose();
	Vec3 cameraDirection = cameraFrame * Vec3(0, 0, 1);

	double distance = (screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (screen.ray * cameraDirection);
	Vec3 planeIntersection = screen.camera.cframe.position + distance * screen.ray;

	if (isPaused()) {
		Vec3 translation = planeIntersection - screen.selectedPoint;
		screen.selectedPoint += translation;
		screen.selectedPart->cframe.translate(translation);
	} else {
		screen.world->selectedPart = screen.selectedPart;
		screen.world->magnetPoint = planeIntersection;
	}
}

void moveGrabbedPhysicalTransversal(Screen& screen, double dz) {
	if(screen.selectedPart == nullptr) return;

	Mat3 cameraFrame = screen.camera.cframe.rotation.transpose();
	Vec3 cameraDirection = cameraFrame * Vec3(0, 0, 1);
	Vec3 cameraYDirection = Vec3(cameraDirection.x, 0, cameraDirection.z).normalize();

	double distance = (screen.selectedPoint - screen.camera.cframe.position) * cameraDirection / (screen.ray * cameraDirection);
	Vec3 planeIntersection = screen.camera.cframe.position + distance * screen.ray;

	Vec3 translation = -cameraYDirection * dz;
	if (isPaused()) {
		screen.selectedPoint += translation;
		screen.selectedPart->cframe.translate(translation);
	} else {
		screen.world->selectedPart = screen.selectedPart;
		screen.world->magnetPoint += translation;
	}
}
