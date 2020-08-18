#pragma once

#include "datastructures/unorderedVector.h"
#include "datastructures/iteratorEnd.h"
#include "part.h"

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
public:
	Part* mainPart;
	UnorderedVector<AttachedPart> parts;
	double mass;            // precomputed cached value for this whole physical
	Vec3 localCenterOfMass; // precomputed cached value for this whole physical
	SymmetricMat3 inertia;  // precomputed cached value for this whole physical


	RigidBody() = default;
	RigidBody(Part* mainPart);

	RigidBody(RigidBody&& other) = default;
	RigidBody& operator=(RigidBody&& other) = default;
	RigidBody(const RigidBody&) = delete;
	void operator=(const RigidBody&) = delete;

	void attach(RigidBody&& otherBody, const CFrame& attachment);
	void attach(Part* part, const CFrame& attachment);
	void detach(Part* part);
	void detach(AttachedPart&& part);
	const AttachedPart& getAttachFor(const Part* attachedPart) const;
	AttachedPart& getAttachFor(const Part* attachedPart);

	inline const Part* getMainPart() const { return mainPart; }
	inline Part* getMainPart() { return mainPart; }

	// returns the offset CFrame from the original mainPart, can be used for syncing calling function
	CFrame makeMainPart(AttachedPart& newMainPart);
	
	void notifyPartStdMoved(Part* oldPartPtr, Part* newPartPtr) noexcept;

	// expects a function of type void(const Part&)
	template<typename Func>
	void forEachAttachedPart(const Func& func) const {
		for(const AttachedPart& atPart : parts) {
			func(static_cast<const Part&>(*atPart.part));
		}
	}
	// expects a function of type void(Part&)
	template<typename Func>
	void forEachAttachedPart(const Func& func) {
		for(const AttachedPart& atPart : parts) {
			func(*atPart.part);
		}
	}
	// expects a function of type void(const Part&)
	template<typename Func>
	void forEachPart(const Func& func) const {
		func(static_cast<const Part&>(*this->mainPart));
		this->forEachAttachedPart(func);
	}
	// expects a function of type void(Part&)
	template<typename Func>
	void forEachPart(const Func& func) {
		func(*this->mainPart);
		this->forEachAttachedPart(func);
	}

	PartIter begin() { 
		if(parts.size() == 0) {
			return PartIter(nullptr, nullptr, mainPart);
		} else {
			return PartIter(&parts[0], &parts[0] + parts.size(), mainPart); 
		}
	}
	ConstPartIter begin() const { 
		if(parts.size() == 0) {
			return ConstPartIter(nullptr, nullptr, mainPart);
		} else {
			return ConstPartIter(&parts[0], &parts[0] + parts.size(), mainPart);
		}
	}

	IteratorEnd end() const { return IteratorEnd(); }

	const Part& operator[](size_t index) const { return (index == 0) ? *mainPart : *parts[index - 1].part; }
	Part& operator[](size_t index) { return (index == 0) ? *mainPart : *parts[index - 1].part; }

	inline size_t getPartCount() const { return parts.size() + 1; }

	void setCFrame(const GlobalCFrame& newCFrame);
	void setCFrameOfPart(Part* part, const GlobalCFrame& newCFrame);
	void translate(const Vec3Fix& translation);
	void rotateAroundLocalPoint(const Vec3& localPoint, const Rotation& rotation);

	void setAttachFor(Part* part, const CFrame& newAttach);

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

	bool isValid() const;

	void refreshWithNewParts();
private:
};
