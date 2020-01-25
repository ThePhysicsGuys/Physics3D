
#include <typeinfo>
#include <typeindex>

#include "serialization.h"
#include "dynamicSerialize.h"

#include "../geometry/normalizedPolyhedron.h"

#include "../constraints/hardConstraint.h"
#include "../constraints/fixedConstraint.h"
#include "../constraints/motorConstraint.h"


void serializeFixedConstraint(const FixedConstraint& object, std::ostream& ostream) {}
FixedConstraint* deserializeFixedConstraint(std::istream& istream) { return new FixedConstraint(); }

void serializeMotorConstraint(const MotorConstraint& constraint, std::ostream& ostream) {
	::serialize<Vec3>(constraint.getAngularVelocity(), ostream);
	::serialize<double>(constraint.getCurrentAngle(), ostream);
}
MotorConstraint* deserializeMotorConstraint(std::istream& istream) {
	Vec3 angularVelocity = ::deserialize<Vec3>(istream);
	double currentAngle = ::deserialize<double>(istream);

	return new MotorConstraint(angularVelocity, currentAngle);
}

void serializeNormalizedPolyhedron(const NormalizedPolyhedron& polyhedron, std::ostream& ostream) {
	::serializePolyhedron(polyhedron, ostream);
}
NormalizedPolyhedron* deserializeNormalizedPolyhedron(std::istream& istream) {
	Polyhedron poly = ::deserializePolyhedron(istream);
	NormalizedPolyhedron* result = new NormalizedPolyhedron(poly.normalized());
	return result;
}

static DynamicSerializerRegistry<HardConstraint>::ConcreteDynamicSerializer<FixedConstraint> fixedConstraintSerializer
	(serializeFixedConstraint, deserializeFixedConstraint, 0);
static DynamicSerializerRegistry<HardConstraint>::ConcreteDynamicSerializer<MotorConstraint> motorConstraintSerializer
	(serializeMotorConstraint, deserializeMotorConstraint, 1);

static DynamicSerializerRegistry<ShapeClass>::ConcreteDynamicSerializer<NormalizedPolyhedron> polyhedronSerializer
	(serializeNormalizedPolyhedron, deserializeNormalizedPolyhedron, 0);

DynamicSerializerRegistry<HardConstraint> hardConstraintRegistry{
	{typeid(FixedConstraint), &fixedConstraintSerializer},
	{typeid(MotorConstraint), &motorConstraintSerializer},
};

DynamicSerializerRegistry<ShapeClass> shapeClassRegistry {
	{typeid(NormalizedPolyhedron), &polyhedronSerializer},
};
