#pragma once

#include "hardConstraint.h"

/*
	Requires a TurnProvider argument, this object must provide the following methods:

	void update(double deltaT);
	void invert();
	double getValue() const;
	FullTaylorExpansion<double, double, NUMBER_OF_DERIVATIVES_IN_MOTION> getFullTaylorExpansion() const;
*/
template<typename TurnProvider>
class MotorConstraintTemplate : public HardConstraint, public TurnProvider {
public:
	using TurnProvider::TurnProvider;

	virtual void update(double deltaT) override { TurnProvider::update(deltaT); }
	virtual void invert() override { TurnProvider::invert(); }

	virtual CFrame getRelativeCFrame() const override {
		return CFrame(Rotation::rotZ(TurnProvider::getValue()));
	}
	virtual RelativeMotion getRelativeMotion() const override {
		FullTaylorExpansion<double, double, NUMBER_OF_DERIVATIVES_IN_MOTION> speedDerivatives = TurnProvider::getFullTaylorExpansion();
		TaylorExpansion<Vec3, NUMBER_OF_DERIVATIVES_IN_MOTION> motorDerivatives;
		for(std::size_t i = 0; i < NUMBER_OF_DERIVATIVES_IN_MOTION; i++) {
			motorDerivatives[i] = Vec3(0, 0, speedDerivatives.derivatives[i]);
		}
		return RelativeMotion(Motion(TranslationalMotion(), RotationalMotion(motorDerivatives)), CFrame(Rotation::rotZ(speedDerivatives.constantValue)));
	}

	virtual ~MotorConstraintTemplate() override {}
};
