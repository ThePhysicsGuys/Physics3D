#include "picker.h"

#include "../util/log.h"
#include "../engine/math/mathUtil.h"

Vec2 getNormalizedDeviceSpacePosition(Vec2 viewportSpacePosition, Vec2 screenSize) {
	double x = 2 * viewportSpacePosition.x / screenSize.x - 1;
	double y = 2 * viewportSpacePosition.y / screenSize.y - 1;
	return Vec2(x, -y);
}

Vec3 calcRay(Vec2 mousePosition, Vec2 screenSize, Mat4f viewMatrix, Mat4f projectionMatrix) {
	Vec2 normalizedDeviceSpacePosition = getNormalizedDeviceSpacePosition(mousePosition, screenSize);
	Vec4f clipSpacePosition = Vec4f(normalizedDeviceSpacePosition.x, normalizedDeviceSpacePosition.y, -1.0f, 1.0f);
	Mat4f invertedProjectionMatrix = projectionMatrix.inverse();
	Vec4f eyeCoordinates = invertedProjectionMatrix * clipSpacePosition;
	eyeCoordinates = Vec4f(eyeCoordinates.x, eyeCoordinates.y, -1.0f, 0.0f);
	Mat4f inverseViewMatrix = viewMatrix.inverse();
	Vec4f rayWorld = inverseViewMatrix * eyeCoordinates;
	Vec3 rayDirection = Vec3(rayWorld.x, rayWorld.y, rayWorld.z).normalize();

	return rayDirection;
}

void updateIntersectedPhysical(Screen* screen, std::vector<Physical>& physicals, Vec3 cameraPosition, Vec2 mousePosition, Vec2 screenSize, Mat4f viewMatrix, Mat4f projectionMatrix) {
	Physical* closestIntersectedPhysical = nullptr;
	Vec3 closestIntersectedPoint = Vec3();
	double closestIntersectDistance = INFINITY;

	Vec3 ray = calcRay(mousePosition, screenSize, viewMatrix, projectionMatrix);

	for (Physical& physical : physicals) {
		Vec3* buffer = new Vec3[physical.part.hitbox.vCount];
		Shape transformed = physical.part.hitbox.localToGlobal(physical.cframe, buffer);
		double distance = transformed.getIntersectionDistance(cameraPosition, ray);
		if (distance < closestIntersectDistance && distance > 0) {
			closestIntersectDistance = distance;
			closestIntersectedPhysical = &physical;
		}
		delete[] buffer;
	}

	if (closestIntersectDistance == INFINITY) {
		closestIntersectedPhysical = nullptr;
		closestIntersectedPoint = cameraPosition;
	} else {
		closestIntersectedPoint = cameraPosition + ray * closestIntersectDistance;
	}

	(*screen->eventHandler.physicalRayIntersectHandler) (screen, closestIntersectedPhysical, closestIntersectedPoint);
}
