#include "camera.h"

#include "../extendedPart.h"

#include "picker/picker.h"
#include "screen.h"

#include <cmath>

void Camera::setPosition(Vec3 position) {
	viewDirty = true;

	cframe.position = position;
}

void Camera::setPosition(double x, double y, double z) {
	setPosition(Vec3(x, y, z));
}

void Camera::setRotation(double alpha, double beta, double gamma) {
	viewDirty = true;

	Mat3 euler = fromEulerAngles(alpha, beta, gamma);
	cframe.rotation = euler;
}

void Camera::setRotation(Vec3 rotation) {
	setRotation(rotation.x, rotation.y, rotation.z);
}

void Camera::rotate(Screen& screen, double dalpha, double dbeta, double dgamma, bool leftDragging) {
	viewDirty = true;

	cframe.rotation = rotX(float(rspeed * dalpha)) * cframe.rotation * rotY(float(rspeed * dbeta));

	if (leftDragging) 
		Picker::moveGrabbedPhysicalLateral(screen);
}

void Camera::rotate(Screen& screen, Vec3 delta, bool leftDragging) {
	rotate(screen, delta.x, delta.y, delta.z, leftDragging);
}

void Camera::move(Screen& screen, double dx, double dy, double dz, bool leftDragging) {
	viewDirty = true;

	(*screen.eventHandler.cameraMoveHandler) (screen, this, Vec3(dx, dy, dz));
	
	Vec3 translation = Vec3();

	if (dx != 0) {
		Vec3 cameraRotationX = cframe.rotation.transpose() * Vec3f(1, 0, 0);
		Vec3 translationX = Vec3(cameraRotationX.x, 0, cameraRotationX.z).normalize() * dx;
		translation += translationX;

		if (leftDragging) 
			Picker::moveGrabbedPhysicalLateral(screen);
	}

	if (dy != 0) {
		Vec3 translationY = Vec3(0, dy, 0);
		translation += translationY;

		if (leftDragging) 
			Picker::moveGrabbedPhysicalLateral(screen);
	}

	if (dz != 0) {
		Vec3 cameraRotationZ = cframe.rotation.transpose() * Vec3f(0, 0, 1);
		Vec3 translationZ = Vec3(cameraRotationZ.x, 0, cameraRotationZ.z).normalize() * dz;
		translation += translationZ;

		if (leftDragging) 
			Picker::moveGrabbedPhysicalTransversal(screen, -speed * dz);
	}

	translation *= speed;

	cframe.translate(translation);
}

void Camera::move(Screen& screen, Vec3 delta, bool leftDragging) {
	move(screen, delta.x, delta.y, delta.z, leftDragging);
}

void Camera::update(float fov, float aspect, float znear, float zfar) {
	this->fov = fov;
	this->aspect = aspect;
	this->znear = znear;
	this->zfar = zfar;

	projectionDirty = true;

	update();
}

void Camera::update(float aspect) {
	this->aspect = aspect;

	projectionDirty = true;
	
	update();
}

void Camera::update() {
	if (!flying && attachment != nullptr) {
		cframe.position = attachment->cframe.position;
		viewDirty = true;
	}

	if (projectionDirty) {
		projectionDirty = false;
		
		projectionMatrix = perspective(fov, aspect, znear, zfar);
		invertedProjectionMatrix = projectionMatrix.inverse();
	}

	if (viewDirty) {
		viewDirty = false;

		viewMatrix = Mat4f(cframe.rotation).translate(-cframe.position);
		invertedViewMatrix = viewMatrix.inverse();
	}
}
