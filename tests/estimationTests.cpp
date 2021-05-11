#include "testsMain.h"

#include "compare.h"
#include "../physics/misc/toString.h"

#include "../physics/geometry/shape.h"
#include "../physics/misc/shapeLibrary.h"

namespace P3D {
TEST_CASE_SLOW(volumeApproximation) {
	Polyhedron s = Library::house;

	BoundingBox b = s.getBounds();

	int sampleCount = 100;

	double dx = (b.max.x - b.min.x) / sampleCount;
	double dy = (b.max.y - b.min.y) / sampleCount;
	double dz = (b.max.z - b.min.z) / sampleCount;

	double sampleVolume = dx * dy * dz;

	long totalContainedCubes = 0;

	for(int xi = 0; xi < sampleCount; xi++) {
		for(int yi = 0; yi < sampleCount; yi++) {
			for(int zi = 0; zi < sampleCount; zi++) {
				double x = b.min.x + dx * xi + dx / 2;
				double y = b.min.y + dy * yi + dy / 2;
				double z = b.min.z + dz * zi + dz / 2;

				if(s.containsPoint(Vec3(x, y, z))) totalContainedCubes++;
			}
		}
	}

	double estimatedVolume = totalContainedCubes * sampleVolume;

	ASSERT_TOLERANT(estimatedVolume == s.getVolume(), s.getVolume() * 0.001);
}

TEST_CASE_SLOW(centerOfMassApproximation) {
	Polyhedron s = Library::house;

	BoundingBox b = s.getBounds();

	int sampleCount = 100;

	double dx = (b.max.x - b.min.x) / sampleCount;
	double dy = (b.max.y - b.min.y) / sampleCount;
	double dz = (b.max.z - b.min.z) / sampleCount;

	double sampleVolume = dx * dy * dz;

	Vec3 total = Vec3(0, 0, 0);

	for(int xi = 0; xi < sampleCount; xi++) {
		for(int yi = 0; yi < sampleCount; yi++) {
			for(int zi = 0; zi < sampleCount; zi++) {
				double x = b.min.x + dx * xi + dx / 2;
				double y = b.min.y + dy * yi + dy / 2;
				double z = b.min.z + dz * zi + dz / 2;

				if(s.containsPoint(Vec3(x, y, z))) {
					total += Vec3(x, y, z) * sampleVolume;
				}
			}
		}
	}

	Vec3 estimatedCOM = total / s.getVolume();

	ASSERT_TOLERANT(estimatedCOM == s.getCenterOfMass(), (dx + dy + dz) / 3 * 0.01);
}

TEST_CASE_SLOW(inertiaApproximation) {

	Polyhedron s = Library::house;

	BoundingBox b = s.getBounds();

	int sampleCount = 100;

	double dx = (b.max.x - b.min.x) / sampleCount;
	double dy = (b.max.y - b.min.y) / sampleCount;
	double dz = (b.max.z - b.min.z) / sampleCount;

	double sampleVolume = dx * dy * dz;

	SymmetricMat3 totalInertia = SymmetricMat3::ZEROS();

	for(int xi = 0; xi < sampleCount; xi++) {
		for(int yi = 0; yi < sampleCount; yi++) {
			for(int zi = 0; zi < sampleCount; zi++) {
				double x = b.min.x + dx * xi + dx / 2;
				double y = b.min.y + dy * yi + dy / 2;
				double z = b.min.z + dz * zi + dz / 2;

				if(s.containsPoint(Vec3(x, y, z))) {
					SymmetricMat3 inertiaOfPoint{
						y * y + z * z,
						-x * y,    x * x + z * z,
						-x * z,     -y * z,   x * x + y * y
					};
					totalInertia += inertiaOfPoint * sampleVolume;
				}
			}
		}
	}

	double v = s.getVolume();
	ASSERT_TOLERANT(totalInertia == s.getInertia(CFrame()), v * v * 0.001);
}
};