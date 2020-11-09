#pragma once

#include "math/linalg/vec.h"
#include "part.h"


class SoftLink{
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

	SoftLink(AttachedPart part1, AttachedPart part2);

	GlobalCFrame getGlobalLocationOfAttach1() const;
	GlobalCFrame getGlobalLocationOfAttach2() const;

	inline GlobalCFrame getCFrameOfPart1() const {
		return this->attachedPart1.part->getCFrame();
	}

	inline GlobalCFrame getCFrameOfPart2() const {
		return this->attachedPart2.part->getCFrame();
	}

	inline Position getPosition2() const {
		return getCFrameOfPart2().getPosition();
	}

	inline Position getPosition1() const {
		return getCFrameOfPart1().getPosition();
	}

};