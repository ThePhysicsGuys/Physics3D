#include "sineWaveController.h"

#include "../../math/predefinedTaylorExpansions.h"
#include "../../math/constants.h"

namespace P3D {
SineWaveController::SineWaveController(double minValue, double maxValue, double period) :
	minValue(minValue),
	maxValue(maxValue),
	period(period),
	currentStepInPeriod(0) {

	assert(maxValue >= minValue);
}

void SineWaveController::update(double deltaT) {
	currentStepInPeriod += deltaT;
	if(currentStepInPeriod >= period) currentStepInPeriod -= period;
}

double SineWaveController::getValue() const {
	double multiplier = (maxValue - minValue) / 2;
	double offset = minValue + multiplier;

	double currentAngle = currentStepInPeriod * (2 * PI / period);

	return sin(currentAngle) * multiplier + offset;
}

FullTaylor<double> SineWaveController::getFullTaylorExpansion() const {
	double multiplier = (maxValue - minValue) / 2;
	double offset = minValue + multiplier;

	double divPeriodToRadians = 2 * PI / period;

	FullTaylor<double> result = generateFullTaylorForSinWave<double, 3>(currentStepInPeriod, (2 * PI / period)) * multiplier;
	result += offset;
	return result;
}
};