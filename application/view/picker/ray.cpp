#include "ray.h"

#include "../screen.h"

#include "../engine/math/vec4.h"
#include "../engine/math/mat4.h"

Vec2f getNormalizedDeviceSpacePosition(Vec2f viewportSpacePosition, Vec2f screenSize) {
	float x = 2 * viewportSpacePosition.x / screenSize.x - 1;
	float y = 2 * viewportSpacePosition.y / screenSize.y - 1;
	return Vec2f(x, -y);
}

Vec3f calcRay(Screen& screen, Vec2f mouse) {
	Vec2f normalizedDeviceSpacePosition = getNormalizedDeviceSpacePosition(mouse, screen.dimension);
	Vec4f clipSpacePosition = Vec4f(normalizedDeviceSpacePosition.x, normalizedDeviceSpacePosition.y, -1.0f, 1.0f);
	Vec4f eyeCoordinates = screen.camera.invertedProjectionMatrix * clipSpacePosition;
	eyeCoordinates = Vec4f(eyeCoordinates.x, eyeCoordinates.y, -1.0f, 0.0f);
	Vec4f rayWorld = screen.camera.invertedViewMatrix * eyeCoordinates;
	Vec3f rayDirection = Vec3f(rayWorld.x, rayWorld.y, rayWorld.z).normalize();

	return rayDirection;
}

Ray getMouseRay(Screen& screen, Vec2f mouse) {
	Position start = screen.camera.cframe.getPosition();
	Vec3f direction = calcRay(screen, mouse);

	return { start, Vec3(direction) };
}