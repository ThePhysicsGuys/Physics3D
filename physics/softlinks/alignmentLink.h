#pragma once

#include "softLink.h"


class AlignmentLink : public SoftLink {
public:
	AlignmentLink(const AlignmentLink& other) = delete;
	AlignmentLink& operator=(const AlignmentLink& other) = delete;
	AlignmentLink(AlignmentLink&& other) = default;
	AlignmentLink& operator=(AlignmentLink&& other) = default;

	AlignmentLink(const AttachedPart& part1, const AttachedPart& part2);
	void update() override;

private:
	[[nodiscard]] Vec3 getGlobalMoment();
	Rotation offset;
	
};
