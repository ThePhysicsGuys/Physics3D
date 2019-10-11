#include "visibilityFilter.h"

#include "../../../util/log.h"
#include "../../math/linalg/trigonometry.h"

VisibilityFilter::VisibilityFilter(Position origin, Vec3 viewPortTop, Vec3 viewPortRight, Vec3 viewPortDown, Vec3 viewPortLeft, Vec3 cameraForward, double maxDepth) :
	origin(origin), 
	boxNormals{viewPortTop, viewPortRight, viewPortDown, viewPortLeft, cameraForward},
	maxDepth(maxDepth) {}

VisibilityFilter::VisibilityFilter(Position origin, Vec3 cameraForward, Vec3 cameraUp, Vec3 cameraRight, double maxDepth) : origin(origin), maxDepth(maxDepth) {
	Vec3 upNormal = (cameraForward + cameraUp) % cameraRight;
	Vec3 downNormal = -(cameraForward - cameraUp) % cameraRight;
	Vec3 leftNormal = -(cameraForward + cameraRight) % cameraUp;
	Vec3 rightNormal = (cameraForward - cameraRight) % cameraUp;

	boxNormals[0] = upNormal;
	boxNormals[1] = rightNormal;
	boxNormals[2] = downNormal;
	boxNormals[3] = leftNormal;
	boxNormals[4] = cameraForward;
}
VisibilityFilter::VisibilityFilter(Position origin, Vec3 cameraForward, Vec3 cameraUp, double fov, double aspect, double maxDepth) :
	VisibilityFilter(
		origin, 
		normalize(cameraForward),
		normalize(cameraUp) * tan(fov / 2),
		normalize(cameraUp % cameraForward) * tan(fov / 2) * aspect,
		maxDepth
	) {}

bool VisibilityFilter::operator()(const TreeNode& node) const {
	double offsets[5]{0,0,0,0,maxDepth};
	for(int i = 0; i < 5; i++) {
		Vec3 normal = boxNormals[i];
		// we're checking that *a* corner of the TreeNode's bounds is within the viewport, basically similar to rectangle-rectangle colissions, google it!
		// cornerOfInterest is the corner that is the furthest positive corner relative to the normal, so if it is not visible (eg above the normal) then the whole box must be invisible
		Position cornerOfInterest(
			(normal.x >= 0) ? node.bounds.min.x : node.bounds.max.x,
			(normal.y >= 0) ? node.bounds.min.y : node.bounds.max.y, // let's look at the top of the viewport, if the bottom of the box is above this then the whole box must be above it. 
			(normal.z >= 0) ? node.bounds.min.z : node.bounds.max.z
		);

		Vec3 relativePos = cornerOfInterest - origin;
		if (relativePos * normal > offsets[i])
			return false;
	}
	return true;
}
/*   A
	/
   / o---o  <-- cornerOfInterest for B
  /  |   |
 /   |   |
o    o---o  <-- cornerOfInterest for A
 \
  \
   \
	\
	 B


A               B
 \       COI B /
  \        v  /
   \       o---o <- COI A
	\      |/  |
	 \     |   |
	  \   /o---o
	   \ /
		o

*/