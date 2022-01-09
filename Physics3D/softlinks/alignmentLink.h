#pragma once

#include "softLink.h"

namespace P3D {
class AlignmentLink : public SoftLink {
public:
	Rotation offset;

	AlignmentLink(const AttachedPart& partA, const AttachedPart& partB);
	~AlignmentLink() override = default;

	AlignmentLink(const AlignmentLink& other) = delete;
	AlignmentLink& operator=(const AlignmentLink& other) = delete;
	AlignmentLink(AlignmentLink&& other) = delete;
	AlignmentLink& operator=(AlignmentLink&& other) = delete;

	void update() override;

private:
	[[nodiscard]] Vec3 getGlobalMoment() noexcept;
};
};