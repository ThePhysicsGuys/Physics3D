#pragma once

#include "../../engine/math/globalCFrame.h"

struct ExtendedPart;
struct Camera;
class Screen;
class Event;
class MouseDragEvent;
class MouseScrollEvent;
class KeyReleaseEvent;

struct Camera {
private:

	enum CameraFlags : char {
		// No flags
		None =				0 << 0,

		// Whether the view matrix needs to be recalculated
		ViewDirty =			1 << 0,

		// Whether the projection matrix needs to be recalculated
		ProjectionDirty =	1 << 1
	};

	char flags = None;

	float fov;
	float znear;
	float zfar;

	double velocityIncrease = 0.071;
	double currentVelocity = 0;
	Vec3 lastDirection = Vec3();	
	bool moving = false;

	double angularVelocityIncrease = 0.15;
	double currentAngularVelocity = 0;
	Vec3 lastRotation = Vec3();	
	bool rotating = false;

	bool wasLeftDragging = false;

	bool onMouseScroll(MouseScrollEvent& event);
	bool onMouseDrag(MouseDragEvent& event);
	bool onKeyRelease(KeyReleaseEvent& event);
public:
	GlobalCFrame cframe;
	double velocity;
	double angularVelocity;
	bool flying;

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
	void onEvent(Event& event);

	Camera(Position position, Mat3 rotation);
	Camera();

	void setPosition(Position position);
	void setPosition(Fix<32> x, Fix<32> y, Fix<32> z);

	void setRotation(double alpha, double beta, double gamma);
	void setRotation(Vec3 rotation);

	void rotate(Screen& screen, double dalpha, double dbeta, double dgamma, bool leftDragging, bool accelerating = true);
	void rotate(Screen& screen, Vec3 delta, bool leftDragging, bool accelerating = true);

	void move(Screen& screen, double dx, double dy, double dz, bool leftDragging, bool accelerating = true);
	void move(Screen& screen, Vec3 delta, bool leftDragging, bool accelerating = true);
};
