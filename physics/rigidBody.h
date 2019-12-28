#pragma once

#include "datastructures/iteratorEnd.h"
#include "part.h"
#include <vector>

struct AttachedPart {
	CFrame attachment = CFrame();
	Part* part = nullptr;
};

struct PartIter {
	AttachedPart* iter;
	AttachedPart* iterEnd;
	Part* first;

	PartIter() = default;
	PartIter(AttachedPart* iter, AttachedPart* iterEnd, Part* first) : iter(iter - 1), iterEnd(iterEnd), first(first) {}

	inline Part& operator*() const {
		return (first != nullptr) ? *first : *iter->part;
	}
	inline void operator++() {
		++iter;
		first = nullptr;
	}
	inline bool operator!=(IteratorEnd) const { return this->iter != this->iterEnd; }
	inline bool operator==(IteratorEnd) const { return this->iter == this->iterEnd; }
};
struct ConstPartIter {
	const AttachedPart* iter;
	const AttachedPart* iterEnd;
	const Part* first;

	ConstPartIter() = default;
	ConstPartIter(const AttachedPart* iter, const AttachedPart* iterEnd, const Part* first) : iter(iter - 1), iterEnd(iterEnd), first(first) {}

	inline const Part& operator*() const {
		return (first != nullptr) ? *first : *iter->part;
	}
	inline void operator++() {
		++iter;
		first = nullptr;
	}
	inline bool operator!=(IteratorEnd) const { return this->iter != this->iterEnd; }
	inline bool operator==(IteratorEnd) const { return this->iter == this->iterEnd; }
};

class RigidBody {
	Part* mainPart;
	std::vector<AttachedPart> parts;
public:
	double mass;            // not part of official state, updated at every tick
	Vec3 localCenterOfMass; // not part of official state, updated at every tick
	SymmetricMat3 inertia;  // not part of official state, updated at every tick


	RigidBody() = default;
	RigidBody(Part* mainPart);

	RigidBody(RigidBody&& other) = default;
	RigidBody& operator=(RigidBody&& other) = default;
	RigidBody(const RigidBody&) = delete;
	void operator=(const RigidBody&) = delete;

	void attach(RigidBody&& otherBody, const CFrame& attachment);
	void attach(Part* part, const CFrame& attachment);
	void detach(AttachedPart& part);
	const AttachedPart& getAttachFor(const Part* attachedPart) const;
	AttachedPart& getAttachFor(const Part* attachedPart);

	// returns the offset CFrame from the original mainPart, can be used for syncing calling function
	CFrame makeMainPart(AttachedPart& newMainPart);


	PartIter begin() { return PartIter(parts.begin()._Ptr, parts.begin()._Ptr + parts.size(), mainPart); }
	ConstPartIter begin() const { return ConstPartIter(parts.begin()._Ptr, parts.begin()._Ptr + parts.size(), mainPart); }

	IteratorEnd end() const { return IteratorEnd(); }

	const Part& operator[](size_t index) const { return (index == 0) ? *mainPart : *parts[index - 1].part; }
	Part& operator[](size_t index) { return (index == 0) ? *mainPart : *parts[index - 1].part; }

	inline size_t getPartCount() const { return parts.size() + 1; }

	void setCFrame(const GlobalCFrame& newCFrame);
	void setPartCFrame(Part* part, const GlobalCFrame& newCFrame);

	inline const GlobalCFrame& getCFrame() const {
		return mainPart->getCFrame();
	}
	inline Position getPosition() const {
		return getCFrame().getPosition();
	}
	inline Position getCenterOfMass() const {
		return getCFrame().localToGlobal(localCenterOfMass);
	}
	inline GlobalCFrame getCenterOfMassCFrame() const {
		return GlobalCFrame(getCFrame().localToGlobal(localCenterOfMass), getCFrame().getRotation());
	}

private:
	void refreshWithNewParts();
};
