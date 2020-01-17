#pragma once

#include <typeinfo>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "../math/fix.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/cframe.h"
#include "../math/globalcframe.h"


#include "../geometry/polyhedron.h"
#include "../geometry/shape.h"

#include "../part.h"
#include "../world.h"
#include "../physical.h"


#include "serializeBasicTypes.h"
#include "dynamicSerialize.h"

#include "../constraints/hardConstraint.h"
#include "../constraints/fixedConstraint.h"
#include "../constraints/motorConstraint.h"

void serializePolyhedron(const Polyhedron& poly, std::ostream& ostream);
Polyhedron deserializePolyhedron(std::istream& istream);

void serializeShape(const Shape& shape, std::ostream& ostream);
Shape deserializeShape(std::istream& istream);

void serializePart(const Part& part, std::ostream& ostream);
Part deserializePart(std::istream& istream);


class PartSerializer {
public:
	virtual void serializePart(const Part& part, std::ostream& ostream);
	virtual Part* deserializePart(std::istream& istream);
};

template<typename ExtendedPart>
class ExtendedPartSerializer : public PartSerializer {
public:
	virtual void serializePart(const Part& part, std::ostream& ostream) final override {
		const ExtendedPart& ePart = static_cast<const ExtendedPart&>(part);
		this->serialize(ePart, ostream);
	}
	virtual Part* deserializePart(std::istream& istream) final override {
		return this->deserialize(istream);
	}

	virtual void serialize(const ExtendedPart& part, std::ostream& ostream) = 0;
	virtual ExtendedPart* deserialize(std::istream& istream) = 0;
};

void serializeMotorizedPhysical(const MotorizedPhysical& phys, std::ostream& ostream, PartSerializer& partSerializer);
MotorizedPhysical* deserializeMotorizedPhysical(std::istream& istream, PartSerializer& partSerializer);

void serializeWorldPrototype(const WorldPrototype& world, std::ostream& ostream, PartSerializer& partSerializer);
void deserializeWorldPrototype(WorldPrototype& world, std::istream& istream, PartSerializer& partSerializer);

template<typename PartType>
void serializeWorld(const World<PartType>& world, std::ostream& ostream, ExtendedPartSerializer<PartType>& partSerializer) {
	serializeWorldPrototype(world, ostream, partSerializer);
}
template<typename PartType>
void deserializeWorld(World<PartType>& world, std::istream& istream, ExtendedPartSerializer<PartType>& partSerializer) {
	deserializeWorldPrototype(world, istream, partSerializer);
}

void serializeFixedConstraint(const FixedConstraint& object, std::ostream& ostream);
FixedConstraint* deserializeFixedConstraint(std::istream& istream);

void serializeMotorConstraint(const MotorConstraint& constraint, std::ostream& ostream);
MotorConstraint* deserializeMotorConstraint(std::istream& istream);

extern DynamicSerializerRegistry<HardConstraint> hardConstraintRegistry;
