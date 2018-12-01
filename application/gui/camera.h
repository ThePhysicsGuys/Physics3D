#pragma once

#include "../../engine/math/vec3.h"
#include "../../engine/math/rot3.h"

class Camera {
public:
	Vec3 position;
	Rot3 rotation;
	double speed;

	Camera(Vec3 position, Rot3 rotation) : position(position), rotation(rotation), speed(0.05) {};
	Camera() : position(Vec3Util::ZERO), rotation(Rot3Util::ZERO), speed(0.05) {};

	void setSpeed(double speed) {
		this->speed = speed;
	}

	void setPosition(double x, double y, double z) {
		position = Vec3(x, y, z);
	}

	void setPosition(Vec3 position) {
		this->position = position;
	}

	void setRotation(double x, double y, double z) {
		rotation = Rot3(x, y, z);
	}

	void setRotation(Rot3 rotation) {
		this->rotation = rotation;
	};

	void move(double dx, double dy, double dz) {
		double x = position.x, y = position.y, z = position.z;
		x += speed * (sin(rotation.y) * -1.0 * dz + sin(rotation.y - 90) * -1.0 * dx);
		z += speed * (cos(rotation.y) * dz + cos(rotation.y - 90) * dx);
		y += speed * dy;
		position = Vec3(x, y, z);
	}
};

