#pragma once

#include "../../engine/math/globalCFrame.h"

struct ExtendedPart;
struct Camera;
class Screen;

struct Camera {
private:
	bool viewDirty = true;
	bool projectionDirty = true;

	float fov;
	float znear;
	float zfar;
	
public:
	GlobalCFrame cframe;
	double speed;
	double rspeed;
	bool flying;

	float aspect;
	
	Mat4f viewMatrix;
	Mat4f invertedViewMatrix;

	Mat4f projectionMatrix;
	Mat4f invertedProjectionMatrix;

	Mat4f orthoMatrix;

	ExtendedPart* attachment = nullptr;

	void update();
	void update(float fov, float aspect, float znear, float zfar);
	void update(float aspect);

	Camera(Position position, Mat3 rotation);
	Camera();

	void setPosition(Position position);
	void setPosition(Fix<32> x, Fix<32> y, Fix<32> z);

	void setRotation(double alpha, double beta, double gamma);
	void setRotation(Vec3 rotation);

	void rotate(Screen& screen, double dalpha, double dbeta, double dgamma, bool leftDragging);
	void rotate(Screen& screen, Vec3 delta, bool leftDragging);

	void move(Screen& screen, double dx, double dy, double dz, bool leftDragging);
	void move(Screen& screen, Vec3 delta, bool leftDragging);
};
