#include "core.h"

#include "camera.h"
#include "screen.h"
#include "input/standardInputHandler.h"
#include "application.h"
#include "../graphics/gui/guiUtils.h"

#include "../engine/event/event.h"
#include "../engine/event/mouseEvent.h"
#include "../engine/event/keyEvent.h"
#include "../engine/input/mouse.h"
#include "../engine/input/keyboard.h"
#include "../engine/options/keyboardOptions.h"

#include "../extendedPart.h"
#include "picker/picker.h"
#include "worlds.h"

namespace P3D::Application {

Camera::Camera(const Position& position, const Rotation& rotation) : cframe(GlobalCFrame(position, rotation)), velocity(0.35), angularVelocity(0.04), flying(true) {
	onUpdate();
};

Camera::Camera() : cframe(GlobalCFrame()), velocity(0.35), angularVelocity(0.04), flying(true) {
	onUpdate();
};

void Camera::setPosition(Position position) {
	flags |= ViewDirty;

	cframe.position = position;
}

void Camera::setPosition(Fix<32> x, Fix<32> y, Fix<32> z) {
	setPosition(Position(x, y, z));
}

void Camera::setRotation(const Rotation& rotation) {
	flags |= ViewDirty;

	cframe.rotation = rotation;
}

void Camera::setRotation(double alpha, double beta, double gamma) {
	this->setRotation(Rotation::fromEulerAngles(alpha, beta, gamma));
}

void Camera::setRotation(Vec3 rotation) {
	setRotation(rotation.x, rotation.y, rotation.z);
}

Mat4f Camera::getViewRotation() {
	return joinDiagonal(Mat3f(cframe.rotation.asRotationMatrix().transpose()), 1.0f);
}

void Camera::rotate(Screen& screen, double dalpha, double dbeta, double dgamma, bool leftDragging, bool accelerating) {
	flags |= ViewDirty;
	rotating = accelerating;

	cframe.rotation = Rotation::rotY(currentAngularVelocity * dbeta) * cframe.rotation * Rotation::rotX(currentAngularVelocity * dalpha);

	if (leftDragging) {
		screen.world->asyncModification([&screen] () {
			Picker::moveGrabbedPhysicalLateral(screen);
			});
	}
	// Accelerate camera rotation
	if (accelerating)
		currentAngularVelocity += angularVelocity * angularVelocityIncrease;

	// Clamp camera angular velocity
	if (currentAngularVelocity > angularVelocity)
		currentAngularVelocity = angularVelocity;

	// Save last rotation
	lastRotation = Vec3(dalpha, dbeta, dgamma);

	// Save left dragging value
	wasLeftDragging = leftDragging;
}

void Camera::rotate(Screen& screen, Vec3 delta, bool leftDragging, bool accelerating) {
	rotate(screen, delta.x, delta.y, delta.z, leftDragging, accelerating);
}

void Camera::move(Screen& screen, double dx, double dy, double dz, bool leftDragging, bool accelerating) {
	flags |= ViewDirty;
	moving = accelerating;

	(*screen.eventHandler.cameraMoveHandler) (screen, this, Vec3(dx, dy, dz));

	Vec3 translation = Vec3();

	if (dx != 0) {
		Vec3 cameraRotationX = cframe.rotation * Vec3(1, 0, 0);
		Vec3 translationX = normalize(Vec3(cameraRotationX.x, 0, cameraRotationX.z)) * dx;
		translation += translationX;

		if (leftDragging) {
			screen.world->asyncModification([&screen] () {
				Picker::moveGrabbedPhysicalLateral(screen);
				});
		}
	}

	if (dy != 0) {
		Vec3 translationY = Vec3(0, dy, 0);
		translation += translationY;

		if (leftDragging) {
			screen.world->asyncModification([&screen] () {
				Picker::moveGrabbedPhysicalLateral(screen);
				});
		}
	}

	if (dz != 0) {
		Vec3 cameraRotationZ = cframe.rotation * Vec3(0, 0, 1);
		Vec3 translationZ = normalize(Vec3(cameraRotationZ.x, 0, cameraRotationZ.z)) * dz;
		translation += translationZ;

		if (leftDragging) {
			screen.world->asyncModification([&screen, dz, this] () {
				Picker::moveGrabbedPhysicalTransversal(screen, -currentVelocity * dz);
				});
		}
	}

	translation *= currentVelocity;

	cframe += translation;

	// Accelerate camera movement
	if (accelerating)
		currentVelocity += velocity * velocityIncrease;

	// Clamp camera velocity
	if (currentVelocity > velocity)
		currentVelocity = velocity;

	// Save last direction
	lastDirection = Vec3(dx, dy, dz);

	// Save left dragging value
	wasLeftDragging = leftDragging;
}

void Camera::move(Screen& screen, Vec3 delta, bool leftDragging, bool accelerating) {
	move(screen, delta.x, delta.y, delta.z, leftDragging, accelerating);
}

bool Camera::onKeyRelease(Engine::KeyReleaseEvent& event) {
	using namespace Engine;
	Key key = event.getKey();

	if (key == KeyboardOptions::Move::forward ||
		key == KeyboardOptions::Move::backward ||
		key == KeyboardOptions::Move::left ||
		key == KeyboardOptions::Move::right ||
		key == KeyboardOptions::Move::ascend ||
		key == KeyboardOptions::Move::descend) {
		moving = false;
	}

	if (key == KeyboardOptions::Rotate::left ||
		key == KeyboardOptions::Rotate::right ||
		key == KeyboardOptions::Rotate::up ||
		key == KeyboardOptions::Rotate::down) {
		rotating = false;
	}

	return false;
}

bool Camera::onMouseDrag(Engine::MouseDragEvent& event) {
	double dmx = event.getNewX() - event.getOldX();
	double dmy = event.getNewY() - event.getOldY();

	// Camera rotating
	if (event.isRightDragging())
		rotate(screen, -dmy * 0.1, -dmx * 0.1, 0, event.isLeftDragging());


	// Camera moving
	if (event.isMiddleDragging())
		move(screen, dmx * 0.2, -dmy * 0.2, 0, event.isLeftDragging());

	return false;
}

bool Camera::onMouseScroll(Engine::MouseScrollEvent& event) {
	velocity = Graphics::GUI::clamp(velocity * (1 + 0.2 * event.getYOffset()), 0.001, 100);

	thirdPersonDistance -= event.getYOffset();

	return true;
};

void Camera::onEvent(Engine::Event& event) {
	using namespace Engine;

	EventDispatcher dispatcher(event);
	dispatcher.dispatch<MouseDragEvent>(BIND_EVENT_METHOD(Camera::onMouseDrag));
	dispatcher.dispatch<MouseScrollEvent>(BIND_EVENT_METHOD(Camera::onMouseScroll));
	dispatcher.dispatch<KeyReleaseEvent>(BIND_EVENT_METHOD(Camera::onKeyRelease));
}

void Camera::onUpdate(float fov, float aspect, float znear, float zfar) {
	this->fov = fov;
	this->aspect = aspect;
	this->znear = znear;
	this->zfar = zfar;

	flags |= ProjectionDirty;

	onUpdate();
}

void Camera::onUpdate(float aspect) {
	this->aspect = aspect;

	flags |= ProjectionDirty;

	orthoMatrix = ortho(-aspect, aspect, -1.0f, 1.0f, -1000.0f, 1000.0f);

	onUpdate();
}

void Camera::onUpdate() {

	if (currentVelocity != 0) {
		// Clamp camera velocity
		if (currentVelocity < 0) {
			currentVelocity = 0;
			wasLeftDragging = false;
		} else {
			// Decellerate camera movement if the camera stops moving
			if (!moving) {
				move(screen, lastDirection, wasLeftDragging, false);
				currentVelocity -= velocity * velocityIncrease;
			}
		}
	}

	if (currentAngularVelocity != 0) {
		// Clamp camera rotation
		if (currentAngularVelocity < 0) {
			currentAngularVelocity = 0;
			wasLeftDragging = false;
		} else {
			// Decellerate camera rotation if the camera stops rotating
			if (!rotating) {
				rotate(screen, lastRotation, wasLeftDragging, false);
				currentAngularVelocity -= angularVelocity * angularVelocityIncrease;
			}
		}
	}

	// Attach the camera to the attached part, if there is anys
	if (!flying && attachment != nullptr) {
		Vec3 vertical = Vec3(0, 1, 0);
		Vec3 forward = getForwardDirection();

		// Sinus angle between camera direction and the xz plane
		double sinAlpha = vertical * forward;

		double dy = thirdPersonDistance * sinAlpha;
		double dz = sqrt(thirdPersonDistance * thirdPersonDistance - dy * dy);

		Vec3 translationZ = normalize(-Vec3(forward.x, 0, forward.z)) * dz;
		Vec3 translationY = Vec3(0, -dy, 0);
		Vec3 translation = translationY + translationZ;

		this->cframe.position = attachment->getCFrame().position + translation;

		flags |= ViewDirty;
	}

	// Update projection matrix
	if (flags & ProjectionDirty) {
		flags ^= ProjectionDirty;

		projectionMatrix = perspective(fov, aspect, znear, zfar);
		invertedProjectionMatrix = ~projectionMatrix;
	}

	// Update view matrix
	if (flags & ViewDirty) {
		flags ^= ViewDirty;

		viewMatrix = translate(getViewRotation(), -Vec3f(float(cframe.position.x), float(cframe.position.y), float(cframe.position.z)));
		invertedViewMatrix = ~viewMatrix;
	}
}

double Camera::getRightOffsetAtZ1() const {
	return tan(double(fov) / 2) * aspect;
}

double Camera::getTopOffsetAtZ1() const {
	return tan(double(fov) / 2);
}

};
