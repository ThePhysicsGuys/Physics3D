#include "core.h"

#include "ray.h"
#include "view/screen.h"

namespace P3D::Application {

Vec2f getNormalizedDeviceSpacePosition(const Vec2f& viewportSpacePosition, const Vec2f& screenSize) {
	float x = 2 * viewportSpacePosition.x / screenSize.x - 1;
	float y = 2 * viewportSpacePosition.y / screenSize.y - 1;
	return Vec2f(x, -y);
}

Vec3f calcRay(const Screen& screen, Vec2f mouse) {
	Vec2f normalizedDeviceSpacePosition = getNormalizedDeviceSpacePosition(mouse, screen.dimension);
	Vec4f clipSpacePosition = Vec4f(normalizedDeviceSpacePosition.x, normalizedDeviceSpacePosition.y, -1.0f, 1.0f);
	Vec4f eyeCoordinates = screen.camera.invertedProjectionMatrix * clipSpacePosition;
	eyeCoordinates = Vec4f(eyeCoordinates.x, eyeCoordinates.y, -1.0f, 0.0f);
	Vec4f rayWorld = screen.camera.invertedViewMatrix * eyeCoordinates;
	Vec3f rayDirection = normalize(Vec3f(rayWorld.x, rayWorld.y, rayWorld.z));

	return rayDirection;
}

Ray getMouseRay(const Screen& screen, const Vec2f& mouse) {
	Position start = screen.camera.cframe.getPosition();
	Vec3f direction = calcRay(screen, mouse);

	return Ray { start, Vec3(direction) };
}

};