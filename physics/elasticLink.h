#pragma once


#include "softLink.h"
#include <optional>


class ElasticLink : public SoftLink {

public:
	ElasticLink(AttachedPart part1, AttachedPart part2, const double restLenght, const double stiffness);
	void update() override;

private:
	std::optional<Vec3> forceAppliedToTheLink();

private:
	double restLength;
	double stiffness;
	
};
