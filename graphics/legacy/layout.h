#pragma once

namespace P3D::Graphics {

class Container;

enum class Align {
	FILL,
	RELATIVE,
	CENTER
};

class Layout {
public:
	// Positions the components of the given container (not including the padding of the container)
	virtual Vec2 resize(Container* container, Vec2 minDimension = Vec2()) = 0;
};

class FlowLayout : public Layout {
public:
	Vec2 resize(Container* container, Vec2 minDimension = Vec2()) override;
};

};