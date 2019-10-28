#pragma once


#include "../../math/bounds.h"
#include "../../datastructures/boundsTree.h"
#include "../../part.h"

class VisibilityFilter {
public:
	Position origin;
private:
	union {
		Vec3 boxNormals[5]; // normals of the viewPort, facing outward
		struct {
			Vec3 up;
			Vec3 down;
			Vec3 left;
			Vec3 right;
			Vec3 forward;
		};
	};
	double maxDepth;
public:
	VisibilityFilter() : boxNormals{} {};
	VisibilityFilter(Position origin, Vec3 normals[5], double maxDepth);
	
	/*
		Creates a VisibilityFilter from the values derived from usual camera parameters.

		cameraForward: the direction of the camera
		cameraUp: should be perpendicular to cameraForward & cameraRight, furthest visible point, starting from cameraForward, in the up direction.
		cameraRight: should be perpendicular to cameraForward & cameraUp, furthest visible point, starting from cameraForward, in the right direction.
	*/
	static VisibilityFilter fromSteps(Position origin, Vec3 cameraForward, Vec3 cameraUp, Vec3 cameraRight, double maxDepth);

	/*
		Creates a VisibilityFilter from the values derived from common camera parameters.

		cameraForward: the direction of the camera
		cameraUp: should be perpendicular to cameraForward & cameraRight, furthest visible point, starting from cameraForward, in the up direction.
		cameraRight: should be perpendicular to cameraForward & cameraUp, furthest visible point, starting from cameraForward, in the right direction.

		Also accepts subWindow parameters:
		left-right goes from -1..1
		down-up goes from -1..1
	*/
	static VisibilityFilter fromSteps(Position origin, Vec3 stepForward, Vec3 stepUp, Vec3 stepRight, double maxDepth, double left, double right, double down, double up);

	/*
		Creates a VisibilityFilter from the values derived from usual camera parameters.

		cameraForward: the direction of the camera
		cameraUp: should be perpendicular to cameraForward, up direction relative to the camera

		fov: Field Of View of the camera, expressed in radians

		aspect: aspect ratio of the camera, == width / height
	*/
	static VisibilityFilter forWindow(Position origin, Vec3 cameraForward, Vec3 cameraUp, double fov, double aspect, double maxDepth);
	
	/*
		Creates a VisibilityFilter for a subregion of the screen. Useful for selection and stuff

		Create it by giving the same arguments as forWindow, but also pass which portion of the screen to select.
		left-right goes from -1..1
		down-up goes from -1..1
	*/
	static VisibilityFilter forSubWindow(Position origin, Vec3 cameraForward, Vec3 cameraUp, double fov, double aspect, double maxDepth, double left, double right, double down, double up);
	
	bool operator()(const TreeNode& node) const;
	bool operator()(const Part& part) const {
		return true;
	}


	inline Vec3 getForwardStep() const { return forward; }
	inline Vec3 getTopOfViewPort() const { return projectToPlaneNormal(forward, up); }
	inline Vec3 getBottomOfViewPort() const { return projectToPlaneNormal(forward, down); }
	inline Vec3 getLeftOfViewPort() const { return projectToPlaneNormal(forward, left); }
	inline Vec3 getRightOfViewPort() const { return projectToPlaneNormal(forward, right); }
};
