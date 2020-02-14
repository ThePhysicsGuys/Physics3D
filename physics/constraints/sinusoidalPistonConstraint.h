#include "hardConstraint.h"

class SinusoidalPistonConstraint : public HardConstraint {
	Vec3 pistonDirection;
	double currentStepInPeriod;
	double minLength;
	double maxLength;
	double period;
public:
	SinusoidalPistonConstraint(Vec3 pistonDirection, double minLength, double maxLength, double period);
	virtual void update(double deltaT) override;
	virtual void invert() override;
	virtual CFrame getRelativeCFrame() const override;
	virtual RelativeMotion getRelativeMotion() const override;
};
