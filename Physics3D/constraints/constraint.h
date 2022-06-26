#pragma once

namespace P3D {
class ConstraintMatrixPack;
struct PhysicalInfo;
struct Constraint {
	virtual int maxNumberOfParameters() const = 0;
	virtual ConstraintMatrixPack getMatrices(const PhysicalInfo& physA, const PhysicalInfo& physB, double* matrixBuf, double* errorBuf) const = 0;
	virtual ~Constraint();
};
}
