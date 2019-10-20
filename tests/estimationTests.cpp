#include "testsMain.h"

#include "../physics/geometry/shape.h"
#include "../physics/misc/shapeLibrary.h"

#include "../physics/math/mathUtil.h"

/*TEST_CASE(volumeApproximation) {
	Polyhedron s = house;

	BoundingBox b = s.getBounds();

	int sampleCount = 100;

	double dx = (b.xmax - b.xmin) / sampleCount;
	double dy = (b.ymax - b.ymin) / sampleCount;
	double dz = (b.zmax - b.zmin) / sampleCount;

	double sampleVolume = dx*dy*dz;

	long totalContainedCubes = 0;

	for(int xi = 0; xi < sampleCount; xi++) {
		for(int yi = 0; yi < sampleCount; yi++) {
			for(int zi = 0; zi < sampleCount; zi++) {
				double x = b.xmin + dx*xi + dx/2;
				double y = b.ymin + dy*yi + dy/2;
				double z = b.zmin + dz*zi + dz/2;

				if(s.containsPoint(Vec3(x, y, z))) totalContainedCubes++;
			}
		}
	}

	double estimatedVolume = totalContainedCubes * sampleVolume;

	ASSERT_TOLERANT(estimatedVolume == s.getVolume(), s.getVolume() * 0.001);
}

TEST_CASE(centerOfMassApproximation) {
	Polyhedron s = house;

	BoundingBox b = s.getBounds();

	int sampleCount = 100;

	double dx = (b.xmax - b.xmin) / sampleCount;
	double dy = (b.ymax - b.ymin) / sampleCount;
	double dz = (b.zmax - b.zmin) / sampleCount;

	double sampleVolume = dx*dy*dz;

	Vec3 total = Vec3(0, 0, 0);

	for(int xi = 0; xi < sampleCount; xi++) {
		for(int yi = 0; yi < sampleCount; yi++) {
			for(int zi = 0; zi < sampleCount; zi++) {
				double x = b.xmin + dx*xi + dx / 2;
				double y = b.ymin + dy*yi + dy / 2;
				double z = b.zmin + dz*zi + dz / 2;

				if(s.containsPoint(Vec3(x, y, z))) {
					total += Vec3(x,y,z) * sampleVolume;
				}
			}
		}
	}

	Vec3 estimatedCOM = total / s.getVolume();

	ASSERT_TOLERANT(estimatedCOM == s.getCenterOfMass(), (dx+dy+dz)/3 * 0.01);
}

TEST_CASE(inertiaApproximation) {

	Polyhedron s = house;

	BoundingBox b = s.getBounds();

	int sampleCount = 100;

	double dx = (b.xmax - b.xmin) / sampleCount;
	double dy = (b.ymax - b.ymin) / sampleCount;
	double dz = (b.zmax - b.zmin) / sampleCount;

	double sampleVolume = dx*dy*dz;

	Mat3 totalInertia = Mat3(0,0,0,0,0,0,0,0,0);

	for(int xi = 0; xi < sampleCount; xi++) {
		for(int yi = 0; yi < sampleCount; yi++) {
			for(int zi = 0; zi < sampleCount; zi++) {
				double x = b.xmin + dx*xi + dx / 2;
				double y = b.ymin + dy*yi + dy / 2;
				double z = b.zmin + dz*zi + dz / 2;

				if(s.containsPoint(Vec3(x, y, z))) {
					Mat3 inertiaOfPoint = Mat3(
						y*y+z*z,  -x*y,    -x*z,
						-x*y,    x*x+z*z,  -y*z,
						-x*z,     -y*z,   x*x+y*y
					);
					totalInertia += inertiaOfPoint * sampleVolume;
				}
			}
		}
	}

	double v = s.getVolume();
	ASSERT_TOLERANT(totalInertia == s.getInertia(), v*v * 0.001);
}*/
