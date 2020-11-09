#pragma once

#include "softLink.h"


class SpringLink : public SoftLink {
public:
	SpringLink(const SpringLink& other) = delete;
	SpringLink& operator=(const SpringLink& other) = delete;
	SpringLink(SpringLink&& other) = default;
	SpringLink& operator=(SpringLink&& other) = default;

	SpringLink(AttachedPart part1, AttachedPart part2,  double length, double preferredLength, double stiffness);

	void update() override;

private:
	Vec3 forceAppliedToTheLink() noexcept;

private:
	double length;
	double preferredLength;
	double stiffness;
};
