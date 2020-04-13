#pragma once

#include "hardConstraint.h"

/*
	Requires a LengthProvider argument, this object must provide the following methods:

	void update(double deltaT);
	void invert();
	double getValue() const;
	FullTaylorExpansion<double, double, NUMBER_OF_DERIVATIVES_IN_MOTION> getFullTaylorExpansion() const;
*/
template<typename LengthProvider>
class PistonConstraintTemplate : public HardConstraint, public LengthProvider {
public:
	using LengthProvider::LengthProvider;

	virtual void update(double deltaT) override { LengthProvider::update(deltaT); }
	virtual void invert() override { LengthProvider::invert(); }

	virtual CFrame getRelativeCFrame() const override {
		return CFrame(0.0, 0.0, LengthProvider::getValue());
	}
	virtual RelativeMotion getRelativeMotion() const override {
		FullTaylorExpansion<double, double, NUMBER_OF_DERIVATIVES_IN_MOTION> speedDerivatives = LengthProvider::getFullTaylorExpansion();
		TaylorExpansion<Vec3, NUMBER_OF_DERIVATIVES_IN_MOTION> pistonDerivatives;
		for(std::size_t i = 0; i < NUMBER_OF_DERIVATIVES_IN_MOTION; i++) {
			pistonDerivatives[i] = Vec3(0, 0, speedDerivatives.derivatives[i]);
		}
		return RelativeMotion(Motion(TranslationalMotion(pistonDerivatives), RotationalMotion()), CFrame(0.0, 0.0, speedDerivatives.constantValue));
	}

	virtual ~PistonConstraintTemplate() override {}
};
