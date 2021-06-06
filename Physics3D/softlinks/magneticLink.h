#pragma once

#include "softLink.h"

namespace P3D {
class MagneticLink : public SoftLink {

public:
	MagneticLink(const MagneticLink& other) = delete;
	MagneticLink& operator=(const MagneticLink& other) = delete;
	MagneticLink(MagneticLink&& other) = default;
	MagneticLink& operator=(MagneticLink&& other) = default;

	MagneticLink(const AttachedPart& part1, const AttachedPart& part2, const double magneticStrength);

	void update() override;

private:
	[[nodiscard]] Vec3 forceAppliedToTheLink() noexcept;

private:
	double magneticStrength;

};
};