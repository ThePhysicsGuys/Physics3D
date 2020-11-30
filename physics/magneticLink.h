#pragma once

#include "softLink.h"

class MagneticLink : public SoftLink {

public:
	MagneticLink(const MagneticLink& other) = delete;
	MagneticLink& operator=(const MagneticLink& other) = delete;
	MagneticLink(MagneticLink&& other) = default;
	MagneticLink& operator=(MagneticLink&& other) = default;

	MagneticLink(AttachedPart part1, AttachedPart part2, const double magneticStrength);

	void update() override;

private:
	Vec3 forceAppliedToTheLink();

private:
	//double restLength;
	double magneticStrength;

};