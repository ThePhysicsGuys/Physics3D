#pragma once

#include "component.h"

#include "../engine/math/mat4.h"
#include "../engine/math/vec3.h"

class DirectionEditor : public Component {
private:
	Mat4f viewMatrix;
	Vec3f viewPosition;
public:
	DirectionEditor(double x, double y, double width, double height);
	DirectionEditor(double x, double y);

	void render() override;
	Vec2 resize() override;

	void press(Vec2 point) override;
	void release(Vec2 point) override;
	void drag(Vec2 dxy) override;
};