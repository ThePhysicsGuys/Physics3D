#include "core.h"

#include "camera.h"
#include "screen.h"
#include "application.h"
#include "gui/guiUtils.h"

#include "event/event.h"
#include "event/mouseEvent.h"
#include "input/mouse.h"

#include "../extendedPart.h"

#include "picker/picker.h"

Camera::Camera(Position position, Mat3 rotation) : cframe(GlobalCFrame(position, rotation)), speed(0.35), rspeed(0.04), flying(true) {
	onUpdate();
};

Camera::Camera() : cframe(GlobalCFrame()), speed(0.35), rspeed(0.04), flying(true) {
	onUpdate();
};

void Camera::setPosition(Position position) {
	viewDirty = true;

	cframe.position = position;
}

void Camera::setPosition(Fix<32> x, Fix<32> y, Fix<32> z) {
	setPosition(Position(x, y, z));
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

	cframe.rotation = rotX(float(rspeed * dalpha)) * Mat3f(cframe.rotation) * rotY(float(rspeed * dbeta));

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
		Vec3 cameraRotationX = cframe.rotation.transpose() * Vec3(1, 0, 0);
		Vec3 translationX = normalize(Vec3(cameraRotationX.x, 0, cameraRotationX.z)) * dx;
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
		Vec3 cameraRotationZ = cframe.rotation.transpose() * Vec3(0, 0, 1);
		Vec3 translationZ = normalize(Vec3(cameraRotationZ.x, 0, cameraRotationZ.z)) * dz;
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

bool Camera::onMouseDrag(MouseDragEvent& event) {
	double dmx = (event.getNewX() - event.getOldX());
	double dmy = (event.getNewY() - event.getOldY());

	// Camera rotating
	if (event.isRightDragging()) {
		rotate(screen, dmy * 0.1, dmx * 0.1, 0, event.isLeftDragging());
	}

	// Camera moving
	if (event.isMiddleDragging()) {
		move(screen, dmx * -0.2, dmy * 0.2, 0, event.isLeftDragging());
	}

	return false;
}

bool Camera::onMouseScroll(MouseScrollEvent& event) {
	speed = GUI::clamp(speed * (1 + 0.2 * event.getYOffset()), 0.001, 100);

	return true;
};

void Camera::onEvent(Event& event) {
	EventDispatcher dispatcher(event);
	dispatcher.dispatch<MouseDragEvent>(BIND_EVENT_METHOD(Camera::onMouseDrag));
	dispatcher.dispatch<MouseScrollEvent>(BIND_EVENT_METHOD(Camera::onMouseScroll));
}

void Camera::onUpdate(float fov, float aspect, float znear, float zfar) {
	this->fov = fov;
	this->aspect = aspect;
	this->znear = znear;
	this->zfar = zfar;

	projectionDirty = true;

	onUpdate();
}

void Camera::onUpdate(float aspect) {
	this->aspect = aspect;

	projectionDirty = true;

	orthoMatrix = ortho(-aspect, aspect, -1.0f, 1.0f, -1000.0f, 1000.0f);

	onUpdate();
}

void Camera::onUpdate() {
	if (!flying && attachment != nullptr) {
		this->cframe.position = attachment->getCFrame().position;
		viewDirty = true;
	}

	if (projectionDirty) {
		projectionDirty = false;
		
		projectionMatrix = perspective(fov, aspect, znear, zfar);
		invertedProjectionMatrix = projectionMatrix.inverse();
	}

	if (viewDirty) {
		viewDirty = false;

		viewMatrix = Mat4f(cframe.rotation).translate(-Vec3f(float(cframe.position.x), float(cframe.position.y), float(cframe.position.z)));
		invertedViewMatrix = viewMatrix.inverse();
	}
}
