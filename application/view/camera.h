#pragma once

struct Camera;
class Screen;

#include "../../engine/math/vec3.h"
#include "../../engine/math/mat3.h"
#include "../../engine/math/mat4.h"
#include "../../engine/math/mathUtil.h"
#include "../../engine/math/cframe.h"
#include "../../engine/part.h"
#include "../extendedPart.h"

#include <cmath>

struct Camera {
private:
	bool viewDirty = true;
	bool projectionDirty = true;

	float fov;
	float znear;
	float zfar;

public:
	CFramef cframe;
	double speed;
	double rspeed;
	bool flying;

	float aspect;
	
	Mat4f viewMatrix;
	Mat4f invertedViewMatrix;

	Mat4f projectionMatrix;
	Mat4f invertedProjectionMatrix;

	ExtendedPart* attachment = nullptr;

	void update();
	void update(float fov, float aspect, float znear, float zfar);
	void update(float aspect);

	Camera(Vec3 position, Mat3 rotation) : cframe(CFramef(position, rotation)), speed(0.35), rspeed(0.04), flying(true) {
		update();
	};

	Camera() : cframe(CFramef()), speed(0.35), rspeed(0.04), flying(true) {
		update();
	};

	void setPosition(Vec3 position);
	void setPosition(double x, double y, double z);

	void setRotation(double alpha, double beta, double gamma);
	void setRotation(Vec3 rotation);

	void rotate(Screen& screen, double dalpha, double dbeta, double dgamma, bool leftDragging);
	void rotate(Screen& screen, Vec3 delta, bool leftDragging);

	void move(Screen& screen, double dx, double dy, double dz, bool leftDragging);
	void move(Screen& screen, Vec3 delta, bool leftDragging);
};
