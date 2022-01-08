#pragma once


#include "softLink.h"
#include <optional>

namespace P3D {
class ElasticLink : public SoftLink {

public:
	double restLength;
	double stiffness;

	ElasticLink(const AttachedPart& partA, const AttachedPart& partB, double restLength, double stiffness);
	~ElasticLink() override = default;

	ElasticLink(const ElasticLink&) = delete;
	ElasticLink& operator=(const ElasticLink&) = delete;
	ElasticLink(ElasticLink&&) = delete;
	ElasticLink& operator=(ElasticLink&&) = delete;

	void update() override;

private:
	std::optional<Vec3> forceAppliedToTheLink();
};
};
