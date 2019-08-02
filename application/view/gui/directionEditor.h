#pragma once

#include "component.h"

#include "../engine/math/mat4.h"
#include "../engine/math/vec3.h"
#include "../engine/math/position.h"

class DirectionEditor;

typedef void (*DirectionEditorAction) (DirectionEditor*);

class DirectionEditor : public Component {
private:
	Mat4f viewMatrix;
	Position viewPosition;
	
	double rspeed;
	void rotate(double dalpha, double dbeta, double dgamma);
public:
	Mat4f modelMatrix;
	DirectionEditorAction action = [] (DirectionEditor*) {};

	DirectionEditor(double x, double y, double width, double height);

	void render() override;
	Vec2 resize() override;

	void drag(Vec2 newPoint, Vec2 oldPoint) override;
};