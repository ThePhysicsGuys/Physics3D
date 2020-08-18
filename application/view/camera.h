#pragma once

#include "../physics/math/rotation.h"
#include "../physics/math/globalCFrame.h"

namespace P3D::Engine {
class Event;
class MouseDragEvent;
class MouseScrollEvent;
class KeyReleaseEvent;
};

namespace P3D::Application {

struct ExtendedPart;
struct Camera;
class Screen;

struct Camera {
private:

	enum CameraFlags : char {
		// No flags
		None = 0 << 0,

		// Whether the view matrix needs to be recalculated
		ViewDirty = 1 << 0,

		// Whether the projection matrix needs to be recalculated
		ProjectionDirty = 1 << 1
	};

	char flags = None;


	double velocityIncrease = 0.5;
	double currentVelocity = 0;
	Vec3 lastDirection = Vec3();
	bool moving = false;

	double angularVelocityIncrease = 0.5;
	double currentAngularVelocity = 0;
	Vec3 lastRotation = Vec3();
	bool rotating = false;

	bool wasLeftDragging = false;

	bool onMouseScroll(Engine::MouseScrollEvent& event);
	bool onMouseDrag(Engine::MouseDragEvent& event);
	bool onKeyRelease(Engine::KeyReleaseEvent& event);

public:
	GlobalCFrame cframe;
	double velocity;
	double angularVelocity;
	bool flying;

	double thirdPersonDistance = 6.0;

	float fov;
	float znear;
	float zfar;
	float aspect;

	Mat4f viewMatrix;
	Mat4f invertedViewMatrix;

	Mat4f projectionMatrix;
	Mat4f invertedProjectionMatrix;

	Mat4f orthoMatrix;

	ExtendedPart* attachment = nullptr;

	void onUpdate();
	void onUpdate(float fov, float aspect, float znear, float zfar);
	void onUpdate(float aspect);
	void onEvent(Engine::Event& event);

	Camera(const Position& position, const Rotation& rotation);
	Camera();

	void setPosition(Position position);
	void setPosition(Fix<32> x, Fix<32> y, Fix<32> z);

	void setRotation(const Rotation& rotation);
	void setRotation(double alpha, double beta, double gamma);
	void setRotation(Vec3 rotation);

	void rotate(Screen& screen, double dalpha, double dbeta, double dgamma, bool leftDragging, bool accelerating = true);
	void rotate(Screen& screen, Vec3 delta, bool leftDragging, bool accelerating = true);

	void move(Screen& screen, double dx, double dy, double dz, bool leftDragging, bool accelerating = true);
	void move(Screen& screen, Vec3 delta, bool leftDragging, bool accelerating = true);

	Mat4f getViewRotation();

	double getRightOffsetAtZ1() const;
	double getTopOffsetAtZ1() const;

	inline Vec3 getForwardDirection() const { return -cframe.rotation.getZ(); }
	inline Vec3 getBackwardDirection() const { return cframe.rotation.getZ(); }
	inline Vec3 getUpDirection() const { return cframe.rotation.getY(); }
	inline Vec3 getDownDirection() const { return -cframe.rotation.getY(); }
	inline Vec3 getLeftDirection() const { return -cframe.rotation.getX(); }
	inline Vec3 getRightDirection() const { return cframe.rotation.getX(); }
};

};