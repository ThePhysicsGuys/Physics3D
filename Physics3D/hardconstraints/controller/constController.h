#pragma once

#include "../../math/taylorExpansion.h"

namespace P3D {
class ConstController {
public:
	double value;
	inline ConstController(double value) : value(value) {}
	inline void update(double deltaT) const {}
	inline double getValue() const { return value; }
	inline FullTaylor<double> getFullTaylorExpansion() const {
		return FullTaylor<double>{value};
	}
};
};