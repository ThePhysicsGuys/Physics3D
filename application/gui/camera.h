#pragma once

#include "../../engine/math/vec3.h"
#include "../../engine/math/rot3.h"
#include "../util/log.h"

#include <cmath>

class Camera {
public:
	Vec3 position;
	Rot3 rotation;
	double speed;
	double rspeed;
	
	Camera(Vec3 position, Rot3 rotation) : position(position), rotation(rotation), speed(0.008), rspeed(0.003) {};
	Camera() : position(Vec3()), rotation(Rot3(0.0,0.0,0.0)), speed(0.008), rspeed(0.003) {};

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

	void rotate(double dx, double dy, double dz) {
		double x = rotation.x, y = rotation.y, z = rotation.z;
		x += rspeed * dx;
		y += rspeed * dy;
		z += rspeed * dz;
		Log::debug("Camera rotation [%f, %f, %f]", x, y, z);
		rotation = Rot3(x, y, z);
	}

	void move(double dx, double dy, double dz) {
		double pi = 3.14159265359;
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
		Log::debug("Camera position [%f, %f, %f]", x, y, z);
		position = Vec3(x, y, z);
	}
};

