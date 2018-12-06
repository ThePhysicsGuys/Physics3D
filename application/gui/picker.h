#pragma once

#include "../../engine/math/mat4.h"
#include "../../engine/math/vec4.h"
#include "../../engine/math/vec3.h"
#include "../../engine/math/vec2.h"
#include "camera.h"

Vec2 getNormalizedDeviceSpacePosition(Vec2 viewportSpacePosition, Vec2 screenSize) {
	double x = 2 * viewportSpacePosition.x / screenSize.x - 1;
	double y = 2 * viewportSpacePosition.y / screenSize.y - 1;
	return Vec2(x, -y);
}

Vec3 calcRay(Vec2 mousePosition, Vec2 screenSize, Camera* camera, Mat4f viewMatrix, Mat4f projectionMatrix) {
	Vec2 normalizedDeviceSpacePosition = getNormalizedDeviceSpacePosition(mousePosition, screenSize);
	Vec4f clipSpacePosition = Vec4f(normalizedDeviceSpacePosition.x, normalizedDeviceSpacePosition.y, -1, 1);
	Mat4f invertedProjectionMatrix = projectionMatrix.inverse();
	Vec4f eyeTempCoordinates = invertedProjectionMatrix * clipSpacePosition;
	Vec4f eyeCoordinates = Vec4f(eyeTempCoordinates.x, eyeTempCoordinates.y, -1, 0);
	Mat4f inverseViewMatrix = viewMatrix.inverse();
	Vec4f worldCoordinates = inverseViewMatrix * eyeCoordinates;
	Vec3 ray = Vec3(worldCoordinates.x, worldCoordinates.y, worldCoordinates.z).normalize();
	return ray;
}