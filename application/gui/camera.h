#pragma once

#include "../../engine/math/vec3.h"
#include "../../engine/math/mat3.h"
#include "../engine/math/cframe.h"
#include "../util/log.h"

#include <cmath>

class Camera {
public:
	CFrame cframe;
	double speed;
	double rspeed;
	
	Camera(Vec3 position, Mat3 rotation) : cframe(CFrame(position, rotation)), speed(0.00008), rspeed(0.0000001) {};
	Camera() : cframe(CFrame()), speed(0.00008), rspeed(0.0000001) {};

	void setPosition(Vec3 position) {
		cframe.position = position;
	}

	void setPosition(double x, double y, double z) {
		setPosition(Vec3(x, y, z));
	}

	void setRotation(double alpha, double beta, double gamma) {
		Mat3 euler = fromEulerAngles(alpha, beta, gamma);
		cframe.rotation = euler;
	}

	void setRotation(Vec3 rotation) {
		setRotation(rotation.x, rotation.y, rotation.z);
	}

	void rotate(double dalpha, double dbeta, double dgamma) {
		cframe.rotation = cframe.rotation * rotX(dalpha) * rotY(dbeta);
	}

	void rotate(Vec3 delta) {
		rotate(delta.x, delta.y, delta.z);
	}

	void move(double dx, double dy, double dz) {
		Vec3 cameraRotationZ = cframe.rotation * Vec3(0, 0, 1);
		Vec3 cameraRotationX = cframe.rotation * Vec3(1, 0, 0);
		Vec3 translation = Vec3(cameraRotationZ.z * dx + cameraRotationX.z * dz, dy, cameraRotationZ.x * dx + cameraRotationX.x * dz);
		cframe.translate(translation);
		/*Vec3 translationxz = cframe.rotation * Vec3(dx, 0, dz);
		Vec3 translation = Vec3(translationxz.x, dy, translationxz.z);
		cframe.translate(translation);*/
		/*double pi = 3.14159265359;
		double x = position.x, y = position.y, z = position.z;
		if (dz != 0) {
			x += speed * sin(rotation.y) * -1.0f * dz;
			z += speed * cos(rotation.y) * dz;
		}
		if (dx != 0) {
			x += speed * sin(rotation.y - pi / 2) * -1.0f * dx;
			z += speed * cos(rotation.y - pi / 2) * dx;
		}
		y += speed * dy;
		//Log::debug("Camera position (%f, %f, %f)", x, y, z);
		position = Vec3(x, y, z);*/
	}

	void move(Vec3 delta) {
		move(delta.x, delta.y, delta.z);
	}
};

