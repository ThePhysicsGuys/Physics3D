#pragma once

class Camera;
class Screen;

#include "../../engine/math/vec3.h"
#include "../../engine/math/mat3.h"
#include "../../engine/math/mathUtil.h"
#include "../../engine/math/cframe.h"
#include "../../engine/part.h"
#include "../extendedPart.h"

#include <cmath>

class Camera {
public:
	CFrame cframe;
	double speed;
	double rspeed;
	bool flying;

	Camera(Vec3 position, Mat3 rotation) : cframe(CFrame(position, rotation)), speed(0.35), rspeed(0.04), flying(true) {};
	Camera() : cframe(CFrame()), speed(0.35), rspeed(0.04), flying(true) {};

	void setPosition(Vec3 position);
	void setPosition(double x, double y, double z);

	void setRotation(double alpha, double beta, double gamma);
	void setRotation(Vec3 rotation);

	void rotate(Screen& screen, double dalpha, double dbeta, double dgamma, bool leftDragging);
	void rotate(Screen& screen, Vec3 delta, bool leftDragging);

	void move(Screen& screen, double dx, double dy, double dz, bool leftDragging);
	void move(Screen& screen, Vec3 delta, bool leftDragging);

	ExtendedPart* attachment = nullptr;
	void update();

};
