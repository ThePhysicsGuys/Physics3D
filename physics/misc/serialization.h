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
#include "../math/globalCFrame.h"
#include "../geometry/polyhedron.h"
#include "../geometry/shape.h"
#include "../part.h"
#include "../world.h"
#include "../physical.h"
#include "../hardconstraints/hardConstraint.h"
#include "../hardconstraints/fixedConstraint.h"
#include "../hardconstraints/motorConstraint.h"

#include "../constraints/ballConstraint.h"

#include "../externalforces/gravityForce.h"

#include "../../util/serializeBasicTypes.h"
#include "../../util/sharedObjectSerializer.h"
#include "../../util/dynamicSerialize.h"


void serializePolyhedron(const Polyhedron& poly, std::ostream& ostream);
Polyhedron deserializePolyhedron(std::istream& istream);

void serializeFixedConstraint(const FixedConstraint& object, std::ostream& ostream);
FixedConstraint* deserializeFixedConstraint(std::istream& istream);

void serializeMotorConstraint(const ConstantSpeedMotorConstraint& constraint, std::ostream& ostream);
ConstantSpeedMotorConstraint* deserializeMotorConstraint(std::istream& istream);

void serializeDirectionalGravity(const DirectionalGravity& gravity, std::ostream& ostream);
DirectionalGravity* deserializeDirectionalGravity(std::istream& istream);


class ShapeSerializer {
public:
	SharedObjectSerializer<const ShapeClass*> sharedShapeClassSerializer;
	ShapeSerializer() = default;
	template<typename List>
	inline ShapeSerializer(const List& knownShapeClasses) : sharedShapeClassSerializer(knownShapeClasses) {}

	void include(const Shape& shape);
	void serializeShape(const Shape& shape, std::ostream& ostream) const;
};
class ShapeDeserializer {
public:
	SharedObjectDeserializer<const ShapeClass*> sharedShapeClassDeserializer;
	ShapeDeserializer() = default;
	template<typename List>
	inline ShapeDeserializer(const List& knownShapeClasses) : sharedShapeClassDeserializer(knownShapeClasses) {}

	Shape deserializeShape(std::istream& ostream) const;
};


class SerializationSessionPrototype {
protected:
	ShapeSerializer shapeSerializer;
	std::map<const Physical*, std::uint32_t> physicalIndexMap;
	std::uint32_t currentPhysicalIndex = 0;

private:
	void collectMotorizedPhysicalInformation(const MotorizedPhysical& motorizedPhys);
	void collectConnectedPhysicalInformation(const ConnectedPhysical& connectedPhys);
	void collectPhysicalInformation(const Physical& phys);

	void serializeMotorizedPhysicalInContext(const MotorizedPhysical& motorizedPhys, std::ostream& ostream);
	void serializePhysicalInContext(const Physical& phys, std::ostream& ostream);
	void serializeRigidBodyInContext(const RigidBody& rigidBody, std::ostream& ostream);

	void serializeWorldLayer(const WorldLayer& layer, std::ostream& ostream);
	void serializeConstraintInContext(const PhysicalConstraint& constraint, std::ostream& ostream);

protected:
	virtual void collectPartInformation(const Part& part);
	virtual void serializeCollectedHeaderInformation(std::ostream& ostream);

	// serializes everything but the part's cframe and layer index, to be done by the calling code as needed
	// calls serializePartExternalData for extending this serialization
	void serializePartData(const Part& part, std::ostream& ostream);
	virtual void serializePartExternalData(const Part& part, std::ostream& ostream);
public:
	/*initializes the SerializationSession with the given ShapeClasses as "known" at deserialization, making it unneccecary to serialize them. 
	Implicitly the builtin ShapeClasses from the physics engine, such as cubeClass and sphereClass are also included in this list */
	SerializationSessionPrototype(const std::vector<const ShapeClass*>& knownShapeClasses = std::vector<const ShapeClass*>());

	void serializeWorld(const WorldPrototype& world, std::ostream& ostream);
	void serializeParts(const Part* const parts[], size_t partCount, std::ostream& ostream);
};

class DeSerializationSessionPrototype {
private:
	MotorizedPhysical* deserializeMotorizedPhysicalWithContext(std::vector<ColissionLayer>& layers, std::istream& istream);
	void deserializeConnectionsOfPhysicalWithContext(std::vector<ColissionLayer>& layers, Physical& physToPopulate, std::istream& istream);
	RigidBody deserializeRigidBodyWithContext(const GlobalCFrame& cframeOfMain, std::vector<ColissionLayer>& layers, std::istream& istream);
	PhysicalConstraint deserializeConstraintInContext(std::istream& istream);
	void deserializeWorldLayer(WorldLayer& layer, std::istream& istream);
protected:
	ShapeDeserializer shapeDeserializer;
	std::vector<Physical*> indexToPhysicalMap;

	// creates a part with the given cframe, layer, and extra data it deserializes
	// calls deserializePartExternalData for extending this deserialization
	Part* deserializePartData(const GlobalCFrame& cframe, WorldLayer* layer, std::istream& istream);
	virtual Part* deserializePartExternalData(Part&& part, std::istream& istream);

	virtual void deserializeAndCollectHeaderInformation(std::istream& istream);

public:
	/*initializes the DeSerializationSession with the given ShapeClasses as "known" at deserialization, these are used along with the deserialized ShapeClasses
	Implicitly the builtin ShapeClasses from the physics engine, such as cubeClass and sphereClass are also included in this list */
	DeSerializationSessionPrototype(const std::vector<const ShapeClass*>& knownShapeClasses = std::vector<const ShapeClass*>());


	void deserializeWorld(WorldPrototype& world, std::istream& istream);
	std::vector<Part*> deserializeParts(std::istream& istream);
};

template<typename NewType, typename OriginalType>
std::vector<NewType*> castVector(std::vector<OriginalType*>&& old) {
	std::vector<NewType*> result(old.size());
	for(size_t i = 0; i < old.size(); i++) {
		result[i] = static_cast<NewType*>(old[i]);
	}
	return result;
}

template<typename ExtendedPartType>
class SerializationSession : private SerializationSessionPrototype {
protected:
	using SerializationSessionPrototype::shapeSerializer;
	using SerializationSessionPrototype::serializeCollectedHeaderInformation;

	virtual void collectExtendedPartInformation(const ExtendedPartType& part) {}
	virtual void serializePartExternalData(const ExtendedPartType& part, std::ostream& ostream) {}

private:
	virtual void collectPartInformation(const Part& part) final override {
		SerializationSessionPrototype::collectPartInformation(part);
		const ExtendedPartType& p = static_cast<const ExtendedPartType&>(part);
		collectExtendedPartInformation(p);
	}

	virtual void serializePartExternalData(const Part& part, std::ostream& ostream) final override {
		const ExtendedPartType& p = static_cast<const ExtendedPartType&>(part);
		serializePartExternalData(p, ostream);
	}
public:
	using SerializationSessionPrototype::SerializationSessionPrototype;


	void serializeWorld(const World<ExtendedPartType>& world, std::ostream& ostream) {
		SerializationSessionPrototype::serializeWorld(world, ostream);
	}

	void serializeParts(const ExtendedPartType* const parts[], size_t partCount, std::ostream& ostream) {
		std::vector<const Part*> baseParts(partCount);
		for(size_t i = 0; i < partCount; i++) {
			baseParts[i] = parts[i];
		}

		SerializationSessionPrototype::serializeParts(&baseParts[0], partCount, ostream);
	}
};

template<typename ExtendedPartType>
class DeSerializationSession : private DeSerializationSessionPrototype {
protected:
	using DeSerializationSessionPrototype::shapeDeserializer;
	using DeSerializationSessionPrototype::deserializeAndCollectHeaderInformation;
	
	virtual ExtendedPartType* deserializeExtendedPart(Part&& partPrototype, std::istream& istream) = 0;

private:
	inline virtual Part* deserializePartExternalData(Part&& part, std::istream& istream) final override { return deserializeExtendedPart(std::move(part), istream); }

public:
	using DeSerializationSessionPrototype::DeSerializationSessionPrototype;

	void deserializeWorld(World<ExtendedPartType>& world, std::istream& istream) { DeSerializationSessionPrototype::deserializeWorld(world, istream); }
	std::vector<ExtendedPartType*> deserializeParts(std::istream& istream) {
		return castVector<ExtendedPartType>(DeSerializationSessionPrototype::deserializeParts(istream));
	}
};

extern DynamicSerializerRegistry<HardConstraint> dynamicHardConstraintSerializer;
extern DynamicSerializerRegistry<ShapeClass> dynamicShapeClassSerializer;
extern DynamicSerializerRegistry<ExternalForce> dynamicExternalForceSerializer;
extern DynamicSerializerRegistry<Constraint> dynamicConstraintSerializer;
