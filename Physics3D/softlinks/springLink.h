#pragma once

#include "softLink.h"

namespace P3D {
class SpringLink : public SoftLink {
public:
	double restLength;
	double stiffness;

	SpringLink(const AttachedPart& partA, const AttachedPart& partB, double restLength, double stiffness);
	~SpringLink() override = default;

	SpringLink(const SpringLink& other) = delete;
	SpringLink& operator=(const SpringLink& other) = delete;
	SpringLink(SpringLink&& other) = delete;
	SpringLink& operator=(SpringLink&& other) = delete;

	void update() override;

private:
	[[nodiscard]] Vec3 forceAppliedToTheLink() noexcept;

};
};