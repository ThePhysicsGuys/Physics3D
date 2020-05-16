#pragma once

#include "../../math/linalg/vec.h"
#include "../../math/bounds.h"
#include "../../datastructures/boundsTree.h"
#include "../../part.h"

class VisibilityFilter {
public:
	Position origin;
private:
	// normals of the viewPort, facing outward
	Vec3 up;
	Vec3 down;
	Vec3 left;
	Vec3 right;
	Vec3 forward;
	double maxDepth;
public:
	VisibilityFilter() : up(), down(), left(), right(), forward(), maxDepth() {};
	VisibilityFilter(const Position& origin, Vec3 normals[5], double maxDepth);
	
	/*
		Creates a VisibilityFilter from the values derived from usual camera parameters.

		stepForward: the direction of the camera
		stepUp: should be perpendicular to stepForward & stepRight, furthest visible point, starting from stepForward, in the up direction.
		stepRight: should be perpendicular to stepForward & stepUp, furthest visible point, starting from stepForward, in the right direction.
	*/
	static VisibilityFilter fromSteps(const Position& origin, const Vec3& stepForward, const Vec3& stepUp, const Vec3& stepRight, double maxDepth);

	/*
		Creates a VisibilityFilter from the values derived from common camera parameters.

		cameraForward: the direction of the camera
		cameraUp: should be perpendicular to cameraForward & cameraRight, furthest visible point, starting from cameraForward, in the up direction.
		cameraRight: should be perpendicular to cameraForward & cameraUp, furthest visible point, starting from cameraForward, in the right direction.

		Also accepts subWindow parameters:
		left-right goes from -1..1
		down-up goes from -1..1
	*/
	static VisibilityFilter fromSteps(const Position& origin, const Vec3& stepForward, const Vec3& stepUp, const Vec3& stepRight, double maxDepth, double left, double right, double down, double up);

	/*
		Creates a VisibilityFilter from the values derived from usual camera parameters.

		cameraForward: the direction of the camera
		cameraUp: should be perpendicular to cameraForward, up direction relative to the camera

		fov: Field Of View of the camera, expressed in radians

		aspect: aspect ratio of the camera, == width / height
	*/
	static VisibilityFilter forWindow(const Position& origin, const Vec3& cameraForward, const Vec3& cameraUp, double fov, double aspect, double maxDepth);
	
	/*
		Creates a VisibilityFilter for a subregion of the screen. Useful for selection and stuff

		Create it by giving the same arguments as forWindow, but also pass which portion of the screen to select.
		left-right goes from -1..1
		down-up goes from -1..1
	*/
	static VisibilityFilter forSubWindow(const Position& origin, const Vec3& cameraForward, const Vec3& cameraUp, double fov, double aspect, double maxDepth, double left, double right, double down, double up);
	
	bool operator()(const TreeNode& node) const;
	bool operator()(const Position& point) const;
	bool operator()(const Part& part) const;

	inline Vec3 getForwardStep() const { return forward; }
	inline Vec3 getTopOfViewPort() const { return projectToPlaneNormal(forward, up); }
	inline Vec3 getBottomOfViewPort() const { return projectToPlaneNormal(forward, down); }
	inline Vec3 getLeftOfViewPort() const { return projectToPlaneNormal(forward, left); }
	inline Vec3 getRightOfViewPort() const { return projectToPlaneNormal(forward, right); }
};
