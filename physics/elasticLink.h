#pragma once


#include "softLink.h"
#include <optional>


class ElasticLink : public SoftLink {

public:
	ElasticLink(const ElasticLink& other) = delete;
	ElasticLink& operator=(const ElasticLink& other) = delete;
	ElasticLink(ElasticLink&& other) = default;
	ElasticLink& operator=(ElasticLink&& other) = default;

	ElasticLink(AttachedPart part1, AttachedPart part2, const double restLenght, const double stiffness);
	void update() override;

private:
	std::optional<Vec3> forceAppliedToTheLink();

private:
	double restLength;
	double stiffness;
	
};
