#pragma once

#include "hardConstraint.h"

/*
	Requires a SpeedController argument, this object must provide the following methods:

	void update(double deltaT);
	double getValue() const;  // returns the current speed of the motor
	FullTaylor<double> getFullTaylorExpansion() const; // returns the current speed and it's derivatives, being acceleration, jerk etc
*/
template<typename SpeedController>
class MotorConstraintTemplate : public HardConstraint, public SpeedController {
public:
	using SpeedController::SpeedController;

	virtual void update(double deltaT) override { SpeedController::update(deltaT); }

	virtual CFrame getRelativeCFrame() const override {
		return CFrame(Rotation::rotZ(SpeedController::getValue()));
	}
	virtual RelativeMotion getRelativeMotion() const override {
		FullTaylor<double> speedDerivatives = SpeedController::getFullTaylorExpansion();
		RotationalMotion rotationMotion(speedDerivatives.getDerivatives().transform([](double v) {return Vec3(0, 0, v); }));
		return RelativeMotion(Motion(TranslationalMotion(), rotationMotion), CFrame(Rotation::rotZ(speedDerivatives.getConstantValue())));
	}

	virtual ~MotorConstraintTemplate() override {}
};

/*
	Requires a LengthController argument, this object must provide the following methods:

	void update(double deltaT);
	double getValue() const; // The current length of the piston
	FullTaylor<double> getFullTaylorExpansion() const; // The current length and its derivatives of the piston 
*/
template<typename LengthController>
class PistonConstraintTemplate : public HardConstraint, public LengthController {
public:
	using LengthController::LengthController;

	virtual void update(double deltaT) override { LengthController::update(deltaT); }

	virtual CFrame getRelativeCFrame() const override {
		return CFrame(0.0, 0.0, LengthController::getValue());
	}
	virtual RelativeMotion getRelativeMotion() const override {
		FullTaylor<double> speedDerivatives = LengthController::getFullTaylorExpansion();
		TranslationalMotion translationMotion(speedDerivatives.getDerivatives().transform([](double v) {return Vec3(0, 0, v); }));
		return RelativeMotion(Motion(translationMotion, RotationalMotion()), CFrame(0.0, 0.0, speedDerivatives.getConstantValue()));
	}

	virtual ~PistonConstraintTemplate() override {}
};
