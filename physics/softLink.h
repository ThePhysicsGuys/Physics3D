#pragma once

#include "math/linalg/vec.h"
#include "part.h"


class SoftLink{
private:
	struct AttachedPart {
		AttachedPart(CFrame attachment, Part* part);
		CFrame attachment = CFrame{};
		Part* part{ nullptr };
	};

	Vec3 forceAppliedToTheLink();

public:
	SoftLink(const SoftLink& other) = delete;
	SoftLink& operator=(const SoftLink& other) = delete;
	SoftLink(SoftLink&& other) = default;
	SoftLink& operator=(SoftLink&& other) = default;

	SoftLink(double stiffness, double length, double preferredLength, AttachedPart part1, AttachedPart part2);
	void update();
	
	GlobalCFrame getCFrameOfPart1() const;
	GlobalCFrame getCFrameOfPart2() const;

	Position getPosition1() const;
	Position getPosition2() const;

	const GlobalCFrame& getGlobalLocationOfAttach1() const;
	const GlobalCFrame& getGlobalLocationOfAttach2() const;


private:
	double stiffness;
	double length;
	double preferredLength;
	AttachedPart attachedPart1;
	AttachedPart attachedPart2;
	
};