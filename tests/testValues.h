#pragma once

#include <Physics3D/math/linalg/vec.h>
#include <Physics3D/math/linalg/mat.h>
#include <Physics3D/math/rotation.h>

namespace P3D {
extern const Vec3 vectors[13];
extern const Vec3 unitVectors[12];
extern const Mat3 matrices[9];
extern const Rotation rotations[19];

// test for bad vertices
extern Vec3f badVerticesI[8];
extern Vec3f badVerticesJ[8];
};

