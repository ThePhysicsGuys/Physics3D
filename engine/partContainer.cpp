#include "partContainer.h"

#include "../util/log.h"
#include <algorithm>
#include "sharedLockGuard.h"

PartContainer::PartContainer(size_t initialCapacity) : physicalCount(0), freePhysicalsOffset(0) {
	physicals = new Physical[initialCapacity];
	parts = new Part*[initialCapacity];
	this->capacity = initialCapacity;
}
void PartContainer::ensureCapacity(size_t targetCapacity) {
	if(this->capacity < targetCapacity) {
		size_t newCapacity = std::max(this->capacity * 2, static_cast<size_t>(32));
		Physical* newPhysicals = new Physical[newCapacity];
		Part** newParts = new Part*[newCapacity];

		for(int i = 0; i < this->capacity; i++) {
			newPhysicals[i] = this->physicals[i];
			newParts[i] = this->parts[i];
			newParts[i]->parent = newPhysicals + i;
		}

		delete[] this->physicals;
		delete[] this->parts;
		this->physicals = newPhysicals;
		this->parts = newParts;
		this->capacity = newCapacity;
		Log::info("Extended physicals capacity to %d", newCapacity);
	}
}
void PartContainer::add(Part* part, bool anchored) {
	ensureCapacity(physicalCount + 1);
	size_t partInsertLocation;
	size_t physicalInsertLocation;
	if(anchored) {
		if(partCount - anchoredPartsCount > 0) movePart(anchoredPartsCount, partCount);
		partInsertLocation = anchoredPartsCount;

		if(physicalCount - freePhysicalsOffset > 0) movePhysical(freePhysicalsOffset, physicalCount);
		physicalInsertLocation = freePhysicalsOffset;

		anchoredPartsCount++;
		freePhysicalsOffset++;
	} else {
		partInsertLocation = partCount;
		physicalInsertLocation = physicalCount;
	}

	parts[partInsertLocation] = part;
	part->partIndex = partInsertLocation;

	physicals[physicalInsertLocation] = Physical(part);
	part->parent = physicals + physicalInsertLocation;

	partCount++;
	physicalCount++;
}

void PartContainer::remove(Part* part) {
	physicalCount--;
	partCount--;
	if(isAnchored(part->parent)) {
		freePhysicalsOffset--;
		anchoredPartsCount--;
		movePhysical(physicals + freePhysicalsOffset, part->parent);
		movePhysical(physicalCount, freePhysicalsOffset);

		movePart(anchoredPartsCount, part->partIndex);
		movePart(partCount, anchoredPartsCount);
	} else {
		movePhysical(physicals + physicalCount, part->parent);
		movePart(partCount, part->partIndex);
	}
}
void PartContainer::anchor(size_t index) {
	anchor(physicals + index);
}
void PartContainer::anchor(Physical* p) {
	if(!isAnchored(p)) {
		swapPhysical(p, physicals + freePhysicalsOffset);
		freePhysicalsOffset++;

		for (AttachedPart& attach : p->parts) {
			swapPart(attach.part->partIndex, anchoredPartsCount);
			anchoredPartsCount++;
		}
	}
}
void PartContainer::unanchor(size_t index) {
	unanchor(physicals + index);
}
void PartContainer::unanchor(Physical* p) {
	if(isAnchored(p)) {
		freePhysicalsOffset--;
		swapPhysical(p, physicals + freePhysicalsOffset);

		for (AttachedPart& attach : p->parts) {
			anchoredPartsCount--;
			swapPart(attach.part->partIndex, anchoredPartsCount);
		}
	}
}
bool PartContainer::isAnchored(size_t index) const {
	return index < freePhysicalsOffset;
}
bool PartContainer::isAnchored(const Physical* physical) const {
	return physical < physicals + freePhysicalsOffset;
}
bool PartContainer::isAnchored(const Part* const * part) const {
	return part < parts + anchoredPartsCount;
}
void PartContainer::swapPhysical(Physical* first, Physical* second) {
	std::swap(*first, *second);
	for (AttachedPart& attach : first->parts)
		attach.part->parent = first;
	
	for (AttachedPart& attach : second->parts)
		attach.part->parent = second;
	
}
void PartContainer::swapPhysical(size_t first, size_t second) {
	swapPhysical(physicals + first, physicals + second);
}
void PartContainer::movePhysical(Physical* origin, Physical* destination) {
	*destination = *origin;
	for (AttachedPart& attach : destination->parts)
		attach.part->parent = destination;
}
void PartContainer::movePhysical(size_t origin, size_t destination) {
	movePhysical(physicals + origin, physicals + destination);
}


void PartContainer::movePart(size_t origin, size_t destination) {
	parts[origin]->partIndex = -1;
	parts[destination] = parts[origin];
	parts[destination]->partIndex = destination;
}

void PartContainer::swapPart(size_t a, size_t b) {
	Part* t = parts[a];
	parts[a] = parts[b];
	parts[b] = t;
	parts[a]->partIndex = a;
	parts[b]->partIndex = b;
}
void PartContainer::swapPart(Part** a, Part** b) {
	Part* t = *a;
	size_t ti = (**a).partIndex;
	(**a).partIndex = (**b).partIndex;
	(**b).partIndex = ti;
	*a = *b;
	*b = t;
}
