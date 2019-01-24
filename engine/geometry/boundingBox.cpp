#include "boundingBox.h"

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

NormalizedShape BoundingBox::toShape(Vec3 vertBuf[8]) const {
	vertBuf[0] = Vec3(xmin, ymin, zmin);
	vertBuf[1] = Vec3(xmax, ymin, zmin);
	vertBuf[2] = Vec3(xmax, ymax, zmin);
	vertBuf[3] = Vec3(xmin, ymax, zmin);
	vertBuf[4] = Vec3(xmin, ymin, zmax);
	vertBuf[5] = Vec3(xmax, ymin, zmax);
	vertBuf[6] = Vec3(xmax, ymax, zmax);
	vertBuf[7] = Vec3(xmin, ymax, zmax);
	return NormalizedShape(vertBuf, triangles, 8, 12);
}
