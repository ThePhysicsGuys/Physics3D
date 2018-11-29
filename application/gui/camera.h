#pragma once

#include "../../engine/math/vec3.h"
#include "../../engine/math/rot3.h"

class Camera {
public:

	Vec3 position;
	Rot3 rotation;

	Camera(Vec3 position, Rot3 rotation) : position(position), rotation(rotation) {};
	Camera() : position(Vec3Util::ZERO), rotation(Rot3Util::ZERO) {};

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
		double xNew, yNew, zNew;
		x += sin(rotation.y) * -1.0 * dz + sin(rotation.y - 90) * -1.0 * dx;
		z += cos(rotation.y) * dz + cos(rotation.y - 90) * dx;
		y += dy;
		position = Vec3(x, y, z);
	}
};

