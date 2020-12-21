#pragma once

#include "math/linalg/vec.h"
#include "part.h"


class SoftLink {
protected:
	struct AttachedPart {
		AttachedPart(CFrame attachment, Part* part);
		CFrame attachment = CFrame{};
		Part* part{ nullptr };
	};

	AttachedPart attachedPart1;
	AttachedPart attachedPart2;

public:
	SoftLink(const SoftLink& other) = delete;
	SoftLink& operator=(const SoftLink& other) = delete;
	SoftLink(SoftLink&& other) = delete;
	SoftLink& operator=(SoftLink&& other) = delete;


	virtual ~SoftLink();
	virtual void update() = 0;

	SoftLink(const AttachedPart& part1, const AttachedPart& part2);

	GlobalCFrame getGlobalCFrameOfAttach1() const;
	GlobalCFrame getGlobalCFrameOfAttach2() const;

	CFrame getLocalCFrameOfAttach1() const;
	CFrame getLocalCFrameOfAttach2() const;

	CFrame getRelativeOfAttach1() const;
	CFrame getRelativeOfAttach2() const;

	Position getGlobalPositionOfAttach1() const;
	Position getGlobalPositionOfAttach2() const;

	Vec3 getLocalPositionOfAttach1() const;
	Vec3 getLocalPositionOfAttach2() const;

	Vec3 getRelativePositionOfAttach1() const;
	Vec3 getRelativePositionOfAttach2() const;

};