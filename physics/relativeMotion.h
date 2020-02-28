#pragma once

#include "math/cframe.h"
#include "motion.h"

struct RelativeMotion {
	Motion relativeMotion;
	CFrame locationOfRelativeMotion;

	RelativeMotion() = default;

	inline RelativeMotion(const Motion& relativeMotion, const CFrame& locationOfRelativeMotion) : 
		relativeMotion(relativeMotion), 
		locationOfRelativeMotion(locationOfRelativeMotion) {}

	/*
		Returns a new RelativeMotion of the new endpoint where the new endpoint is rigidly attached to the old endpoint via extension
		The extension vector is in the local space of the end point
	*/
	inline RelativeMotion extendEnd(const Vec3& extension) const {
		Vec3 relativeExtension = locationOfRelativeMotion.localToRelative(extension);

		Motion resultingMotion = relativeMotion.getMotionOfPoint(relativeExtension);
		
		return RelativeMotion(resultingMotion, locationOfRelativeMotion + relativeExtension);
	}

	/*
		Returns a new RelativeMotion of the new endpoint where the new endpoint is rigidly attached to the old endpoint via extension
		The extension vector is in the local space of the end point
	*/
	inline RelativeMotion extendEnd(const CFrame& extension) const {
		Vec3 relativeExtension = locationOfRelativeMotion.localToRelative(extension.getPosition());

		Motion resultingMotion = relativeMotion.getMotionOfPoint(relativeExtension);

		return RelativeMotion(resultingMotion, locationOfRelativeMotion.localToGlobal(extension));
	}


	/*
		Returns a new RelativeMotion where this relativeMotion has been attached onto an extension
	*/
	inline RelativeMotion extendBegin(const Vec3& extension) const {
		return RelativeMotion(relativeMotion, locationOfRelativeMotion + extension);
	}

	/*
		Returns a new RelativeMotion where this relativeMotion has been attached onto an extension
	*/
	inline RelativeMotion extendBegin(const CFrame& extension) const {
		return RelativeMotion(localToGlobal(extension.getRotation() , relativeMotion), extension.localToGlobal(locationOfRelativeMotion));
	}

	inline Motion applyTo(const Motion& originMotion) const {
		return originMotion.addOffsetRelativeMotion(locationOfRelativeMotion.getPosition(), relativeMotion);
	}
};

/*
	Appends the second RelativeMotion onto the end of the first one
*/
inline RelativeMotion operator+(const RelativeMotion& first, const RelativeMotion& second) {
	CFrame relativeCFrameToBeAdded = first.locationOfRelativeMotion.localToRelative(second.locationOfRelativeMotion);
	
	Motion rotatedSecondMotion = localToGlobal(first.locationOfRelativeMotion.getRotation(), second.relativeMotion);
	Motion motionOfEndpointWithSecondMotion = first.relativeMotion.addOffsetRelativeMotion(relativeCFrameToBeAdded.getPosition(), rotatedSecondMotion);

	CFrame resultingOffset = relativeCFrameToBeAdded + first.locationOfRelativeMotion.getPosition();

	return RelativeMotion(motionOfEndpointWithSecondMotion, resultingOffset);
}
/*
	Appends the second RelativeMotion onto the end of the first one
*/
inline RelativeMotion& operator+=(RelativeMotion& first, const RelativeMotion& second) {
	first = first + second;
	return first;
}