#include "elasticLink.h"
#include <cstdlib>



ElasticLink::ElasticLink(AttachedPart part1, AttachedPart part2, const double restLength, const double stiffness) :
	SoftLink{ part1, part2 },
	restLength{ restLength },
	stiffness{ stiffness } {

}

void ElasticLink::update() {
	auto optionalVec3 = forceAppliedToTheLink();

	if (!optionalVec3) {
		return;
	}

	Vec3 force = optionalVec3.value();
	this->attachedPart1.part->applyForce(this->getRelativePositionOfAttach2(), force);
	this->attachedPart2.part->applyForce(this->getRelativePositionOfAttach2(), -force);
}

std::optional<Vec3> ElasticLink::forceAppliedToTheLink() {
	Vec3 forceVector = this->getGlobalPositionOfAttach1() - this->getGlobalPositionOfAttach2();
	double len = length(forceVector);

	if (len <= this->restLength) {
		return std::nullopt;
	}
	double lenTimesStiffness = std::abs(len - this->restLength) * this->stiffness;
	const Vec3 forceDirection = normalize(forceVector);

	Vec3 force = lenTimesStiffness * -forceDirection;

	return std::optional<Vec3>{force};
}