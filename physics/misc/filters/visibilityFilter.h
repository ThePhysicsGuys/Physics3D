#pragma once


#include "../../math/bounds.h"
#include "../../datastructures/boundsTree.h"
#include "../../part.h"

class VisibilityFilter {
	Position origin;
	Vec3 boxNormals[5]; // normals of the viewPort, facing outward
	double maxDepth;
public:
	VisibilityFilter() = default;
	VisibilityFilter(Position origin, Vec3 viewPortTop, Vec3 viewPortRight, Vec3 viewPortDown, Vec3 viewPortLeft, Vec3 cameraForward, double maxDepth);
	
	/*
		Creates a VisibilityFilter from the values derived from usual camera parameters.

		cameraForward: the direction of the camera
		cameraUp: should be perpendicular to cameraForward & cameraRight, furthest visible point, starting from cameraForward, in the up direction.
		cameraRight: should be perpendicular to cameraForward & cameraUp, furthest visible point, starting from cameraForward, in the right direction.
	*/
	VisibilityFilter(Position origin, Vec3 cameraForward, Vec3 cameraUp, Vec3 cameraRight, double maxDepth);
	
	/*
		Creates a VisibilityFilter from the values derived from usual camera parameters.

		cameraForward: the direction of the camera
		cameraUp: should be perpendicular to cameraForward, up direction relative to the camera

		fov: Field Of View of the camera, expressed in radians

		aspect: aspect ratio of the camera, == width / height
	*/
	VisibilityFilter(Position origin, Vec3 cameraForward, Vec3 cameraUp, double fov, double aspect, double maxDepth);
	
	bool operator()(const TreeNode& node) const;
	bool operator()(const Part& part) const {
		return true;
	}
};
