#pragma once

#include "../../math/taylorExpansion.h"
#include "../../motion.h"

class SineWaveController {
public:
	double currentStepInPeriod;
	double minValue;
	double maxValue;
	double period;

	SineWaveController(double minValue, double maxValue, double period);

	void update(double deltaT);
	double getValue() const;
	FullTaylor<double> getFullTaylorExpansion() const;
};
