#pragma once

#include <typeinfo>
#include <typeindex>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>

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
#include "../constraints/hardConstraint.h"
#include "../constraints/fixedConstraint.h"
#include "../constraints/motorConstraint.h"

#include "../../util/serializeBasicTypes.h"
#include "../../util/dynamicSerialize.h"


void serializePolyhedron(const Polyhedron& poly, std::ostream& ostream);
Polyhedron deserializePolyhedron(std::istream& istream);

typedef uint32_t ShapeClassID;
void serializeShape(const Shape& shape, std::ostream& ostream, const std::map<const ShapeClass*, ShapeClassID>& registry);
Shape deserializeShape(std::istream& istream, const std::map<ShapeClassID, const ShapeClass*>& registry);

void serializeFixedConstraint(const FixedConstraint& object, std::ostream& ostream);
FixedConstraint* deserializeFixedConstraint(std::istream& istream);

void serializeMotorConstraint(const MotorConstraint& constraint, std::ostream& ostream);
MotorConstraint* deserializeMotorConstraint(std::istream& istream);

class PartSerializationInformation;
class PartDeSerializationInformation;
void serializePartWithoutCFrame(const Part& part, std::ostream& ostream, PartSerializationInformation& serializationInformation);
Part deserializePartWithoutCFrame(std::istream& istream, PartDeSerializationInformation& deserializationInformation);
void serializePartWithCFrame(const Part& part, std::ostream& ostream, PartSerializationInformation& serializationInformation);
Part deserializePartWithCFrame(std::istream& istream, PartDeSerializationInformation& deserializationInformation);


class SerializationContextPrototype {
public:
	DynamicSerializerRegistry<HardConstraint> dynamicHardConstraintSerializer;
	DynamicSerializerRegistry<ShapeClass> dynamicShapeClassSerializer;
	std::vector<const ShapeClass*> builtinShapeClasses;

	// automatically initializes this SerializationContextPrototype with builting deserializers
	SerializationContextPrototype();

	virtual void serializePart(const Part& part, std::ostream& ostream, PartSerializationInformation& serializationInformation) const;
	virtual Part* deserializePart(std::istream& istream, PartDeSerializationInformation& deserializationInformation) const;

	void serializeWorld(const WorldPrototype& world, std::ostream& ostream) const;
	void deserializeWorld(WorldPrototype& world, std::istream& istream) const;
};

template<typename ExtendedPartType>
class SerializationContext : private SerializationContextPrototype {
public:
	virtual void serializeExtendedPart(const ExtendedPartType& part, std::ostream& ostream, PartSerializationInformation& serializationInformation) const = 0;
	virtual ExtendedPartType* deserializeExtendedPart(std::istream& istream, PartDeSerializationInformation& deserializationInformation) const = 0;

	virtual void serializePart(const Part& part, std::ostream& ostream, PartSerializationInformation& serializationInformation) const final override {
		const ExtendedPartType& p = static_cast<const ExtendedPartType&>(part);
		serializeExtendedPart(p, ostream, serializationInformation);
	}
	virtual Part* deserializePart(std::istream& istream, PartDeSerializationInformation& deserializationInformation) const final override {
		return deserializeExtendedPart(istream, deserializationInformation);
	}

	void serializeWorld(const World<ExtendedPartType>& world, std::ostream& ostream) const {
		SerializationContextPrototype::serializeWorld(world, ostream);
	}
	void deserializeWorld(World<ExtendedPartType>& world, std::istream& istream) const {
		SerializationContextPrototype::deserializeWorld(world, istream);
	}
};
