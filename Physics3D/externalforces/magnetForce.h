#pragma once

#include "externalForce.h"
#include "../part.h"
#include "../math/linalg/vec.h"
#include "../math/position.h"

namespace P3D {

class MagnetForce : public ExternalForce {
public:
	Part* selectedPart;
	Vec3 localSelectedPoint;
	Position magnetPoint;

	double pickerStrength;
	double pickerSpeedStrength;

	MagnetForce() = default;
	MagnetForce(double pickerStrength, double pickerSpeedStrength);
	MagnetForce(Part& selectedPart, Vec3 localSelectedPoint, Position magnetPoint, double pickerStrength, double pickerSpeedStrength);
	virtual void apply(WorldPrototype* world) override;
};
};
