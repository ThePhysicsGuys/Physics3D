#pragma once


#include "softLink.h"
#include <optional>

namespace P3D {
class ElasticLink : public SoftLink {

public:
	ElasticLink(const ElasticLink&) = delete;
	ElasticLink& operator=(const ElasticLink&) = delete;
	ElasticLink(ElasticLink&&) = default;
	ElasticLink& operator=(ElasticLink&&) = default;

	ElasticLink(AttachedPart part1, AttachedPart part2, const double restLength, const double stiffness);
	void update() override;

private:
	std::optional<Vec3> forceAppliedToTheLink();

private:
	double restLength;
	double stiffness;
};
};
