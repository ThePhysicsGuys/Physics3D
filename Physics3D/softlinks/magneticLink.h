#pragma once

#include "softLink.h"

namespace P3D {
class MagneticLink : public SoftLink {
public:
	double magneticStrength;

	MagneticLink(const AttachedPart& partA, const AttachedPart& partB, double magneticStrength);
	~MagneticLink() override = default;

	MagneticLink(const MagneticLink& other) = delete;
	MagneticLink& operator=(const MagneticLink& other) = delete;
	MagneticLink(MagneticLink&& other) = delete;
	MagneticLink& operator=(MagneticLink&& other) = delete;

	void update() override;

private:
	[[nodiscard]] Vec3 forceAppliedToTheLink() noexcept;

};
};