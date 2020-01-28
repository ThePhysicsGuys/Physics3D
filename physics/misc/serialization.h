#pragma once

#include <typeinfo>
#include <typeindex>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
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

void serializePartWithoutCFrame(const Part& part, std::ostream& ostream, const std::map<const ShapeClass*, ShapeClassID>& classToIDMap);
Part deserializePartWithoutCFrame(std::istream& istream, const std::map<ShapeClassID, const ShapeClass*>& IDToClassMap);
void serializePartWithCFrame(const Part& part, std::ostream& ostream, const std::map<const ShapeClass*, ShapeClassID>& classToIDMap);
Part deserializePartWithCFrame(std::istream& istream, const std::map<ShapeClassID, const ShapeClass*>& IDToClassMap);

class SerializationSessionPrototype {
protected:
	std::map<const ShapeClass*, unsigned int> shapeClassToIDMap;
	std::set<const ShapeClass*> shapeClassesToSerialize;


	void collectMotorizedPhysicalInformation(const MotorizedPhysical& motorizedPhys);
	void collectConnectedPhysicalInformation(const ConnectedPhysical& connectedPhys);
	void collectPhysicalInformation(const Physical& phys);

	virtual void collectPartInformation(const Part& part);

	virtual void serializeCollectedHeaderInformation(std::ostream& ostream);

	void serializeMotorizedPhysical(const MotorizedPhysical& motorizedPhys, std::ostream& ostream);
	void serializePhysical(const Physical& phys, std::ostream& ostream);
	void serializeRigidBody(const RigidBody& rigidBody, std::ostream& ostream);

	virtual void serializePart(const Part& part, std::ostream& ostream);
public:
	/*initializes the SerializationSession with the given ShapeClasses as "known" at deserialization, making it unneccecary to serialize them. 
	Implicitly the builtin ShapeClasses from the physics engine, such as cubeClass and sphereClass are also included in this list */
	SerializationSessionPrototype(const std::vector<const ShapeClass*>& knownShapeClasses = std::vector<const ShapeClass*>());

	void serializeWorld(const WorldPrototype& world, std::ostream& ostream);
};

class DeSerializationSessionPrototype {
protected:
	std::map<unsigned int, const ShapeClass*> IDToShapeClassMap;

	virtual void deserializeAndCollectHeaderInformation(std::istream& istream);

	MotorizedPhysical* deserializeMotorizedPhysical(std::istream& istream);
	void deserializeConnectionsOfPhysical(Physical& physToPopulate, std::istream& istream);
	RigidBody deserializeRigidBody(std::istream& istream);

	virtual Part* deserializePart(std::istream& istream);

public:
	/*initializes the DeSerializationSession with the given ShapeClasses as "known" at deserialization, these are used along with the deserialized ShapeClasses
	Implicitly the builtin ShapeClasses from the physics engine, such as cubeClass and sphereClass are also included in this list */
	DeSerializationSessionPrototype(const std::vector<const ShapeClass*>& knownShapeClasses = std::vector<const ShapeClass*>());


	void deserializeWorld(WorldPrototype& world, std::istream& istream);
};


/*TODO: add information collection methods to serializationSession, such as visitPart, and serializePrerequisiteInformation
These can be overridden to plug custom features into the serialization
TODO: move PartSerializationInformation into SerializationSession*/



template<typename ExtendedPartType>
class SerializationSession : private SerializationSessionPrototype {
protected:
	using SerializationSessionPrototype::shapeClassToIDMap;
	virtual void serializeExtendedPart(const ExtendedPartType& part, std::ostream& ostream) = 0;

	virtual void serializePart(const Part& part, std::ostream& ostream) final override {
		const ExtendedPartType& p = static_cast<const ExtendedPartType&>(part);
		serializeExtendedPart(p, ostream);
	}

public:
	using SerializationSessionPrototype::SerializationSessionPrototype;


	void serializeWorld(const World<ExtendedPartType>& world, std::ostream& ostream) {
		SerializationSessionPrototype::serializeWorld(world, ostream);
	}
};

template<typename ExtendedPartType>
class DeSerializationSession : private DeSerializationSessionPrototype {
protected:
	using DeSerializationSessionPrototype::IDToShapeClassMap;
	virtual ExtendedPartType* deserializeExtendedPart(std::istream& istream) = 0;

	virtual Part* deserializePart(std::istream& istream) final override {
		return deserializeExtendedPart(istream);
	}

public:
	using DeSerializationSessionPrototype::DeSerializationSessionPrototype;


	void deserializeWorld(World<ExtendedPartType>& world, std::istream& istream) {
		DeSerializationSessionPrototype::deserializeWorld(world, istream);
	}
};

extern DynamicSerializerRegistry<HardConstraint> dynamicHardConstraintSerializer;
extern DynamicSerializerRegistry<ShapeClass> dynamicShapeClassSerializer;
