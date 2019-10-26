#pragma once

class Part;
class Physical;
class WorldPrototype;
#include "geometry/shape.h"
#include "math/linalg/mat.h"
#include "math/position.h"
#include "math/globalCFrame.h"
#include "math/bounds.h"

struct PartProperties {
	double density;
	double friction;
	double bouncyness;
};

struct PartIntersection {
	bool intersects;
	Position intersection;
	Vec3 exitVector;

	PartIntersection() : intersects(false) {}
	PartIntersection(const Position& intersection, const Vec3& exitVector) :
		intersects(true),
		intersection(intersection),
		exitVector(exitVector) {}
};


class Part {
	friend class Physical;
	friend class WorldPrototype;

	GlobalCFrame cframe;

public:
	bool isTerrainPart = false;
	Physical* parent = nullptr;
	Shape hitbox;
	double maxRadius;
	PartProperties properties;

	double mass;
	SymmetricMat3 inertia;
	Vec3 localCenterOfMass;

	BoundingBox localBounds;

	/*
		This is extra velocity that should be added to any colission
		if this part is anchored, this gives the velocity of another part sliding on top of it, with perfect friction

		In other words, this is the desired relative velocity for there to be no friction
	*/
	Vec3 conveyorEffect = Vec3(0, 0, 0);

	Part() = default;
	Part(const Shape& shape, const GlobalCFrame& position, const PartProperties& properties);
	~Part();
	PartIntersection intersects(const Part& other) const;
	void scale(double scaleX, double scaleY, double scaleZ);

	Bounds getStrictBounds() const;

	Position getPosition() const { return cframe.getPosition(); }
	Position getCenterOfMass() const { return cframe.localToGlobal(localCenterOfMass); }
	const GlobalCFrame& getCFrame() const { return cframe; }
	void setCFrame(const GlobalCFrame& newCFrame);

	void attach(Part& other, const CFrame& relativeCFrame);
	void detach();
};
