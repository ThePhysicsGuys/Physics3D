#include "hardConstraint.h"

#include "pistonConstraintTemplate.h"

class PistonSineGenerator {
public:
	double currentStepInPeriod;
	double minLength;
	double maxLength;
	double period;

	PistonSineGenerator(double minLength, double maxLength, double period);

	void update(double deltaT);
	void invert();
	double getValue() const;
	FullTaylorExpansion<double, double, NUMBER_OF_DERIVATIVES_IN_MOTION> getFullTaylorExpansion() const;
};

typedef PistonConstraintTemplate<PistonSineGenerator> SinusoidalPistonConstraint;

/*class SinusoidalPistonConstraint : public HardConstraint {
public:
	double currentStepInPeriod;
	double minLength;
	double maxLength;
	double period;
	SinusoidalPistonConstraint(double minLength, double maxLength, double period);
	virtual void update(double deltaT) override;
	virtual void invert() override;
	virtual CFrame getRelativeCFrame() const override;
	virtual RelativeMotion getRelativeMotion() const override;
};*/
