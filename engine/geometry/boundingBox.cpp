#include "boundingBox.h"

#include "polyhedron.h"

Triangle triangles[12] = {
	{1,0,2},{3,2,0}, // BOTTOM
	{1,5,0},{4,0,5}, // FRONT
	{1,2,5},{6,5,2}, // RIGHT
	{6,2,7},{3,7,2}, // BACK
	{3,0,7},{4,7,0}, // LEFT
	{4,5,7},{6,7,5}, // TOP
};

bool BoundingBox::intersects(BoundingBox o) const {
	return 
		xmin <= o.xmax && xmax >= o.xmin &&
		ymin <= o.ymax && ymax >= o.ymin &&
		zmin <= o.zmax && zmax >= o.zmin;
}

bool BoundingBox::containsPoint(Vec3 point) const {
	return 
		point.x >= xmin && point.x <= xmax &&
		point.y >= ymin && point.y <= ymax &&
		point.z >= zmin && point.z <= zmax;
}

Polyhedron BoundingBox::toShape() const {
	float xminf = static_cast<float>(xmin);
	float xmaxf = static_cast<float>(xmax);
	float yminf = static_cast<float>(ymin);
	float ymaxf = static_cast<float>(ymax);
	float zminf = static_cast<float>(zmin);
	float zmaxf = static_cast<float>(zmax);

	Vec3f vertBuf[8];

	vertBuf[0] = Vec3f(xminf, yminf, zminf);
	vertBuf[1] = Vec3f(xmaxf, yminf, zminf);
	vertBuf[2] = Vec3f(xmaxf, ymaxf, zminf);
	vertBuf[3] = Vec3f(xminf, ymaxf, zminf);
	vertBuf[4] = Vec3f(xminf, yminf, zmaxf);
	vertBuf[5] = Vec3f(xmaxf, yminf, zmaxf);
	vertBuf[6] = Vec3f(xmaxf, ymaxf, zmaxf);
	vertBuf[7] = Vec3f(xminf, ymaxf, zmaxf);
	return Polyhedron(vertBuf, triangles, 8, 12);
}
