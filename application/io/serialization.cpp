#include "core.h"

#include "serialization.h"

#include "extendedPart.h"
#include "../physics/world.h"
#include "../worlds.h"

#include "../physics/misc/gravityForce.h"
#include "../physics/misc/serialization.h"

#include <fstream>
#include <sstream>

namespace Application {

static void serializeMaterial(const Material& material, std::ostream& ostream) {
	::serialize<Color>(material.ambient, ostream);
	::serialize<Color3>(material.diffuse, ostream);
	::serialize<Color3>(material.specular, ostream);
	::serialize<float>(material.reflectance, ostream);
}

static Material deserializeMaterial(std::istream& istream) {
	Color ambient = ::deserialize<Color>(istream);
	Color3 diffuse = ::deserialize<Color3>(istream);
	Color3 specular = ::deserialize<Color3>(istream);
	float reflectance = ::deserialize<float>(istream);

	return Material(ambient, diffuse, specular, reflectance);
}

class Serializer : public SerializationSession<ExtendedPart> {
public:
	using SerializationSession<ExtendedPart>::SerializationSession;
	virtual void serializeExtendedPart(const ExtendedPart& part, std::ostream& ostream) override {
		::serializePartWithoutCFrame(part, ostream, this->shapeClassToIDMap);
		serializeMaterial(part.material, ostream);
		::serialize<int>(part.renderMode, ostream);
		::serializeString(part.name, ostream);
	}
};

class Deserializer : public DeSerializationSession<ExtendedPart> {
public:
	using DeSerializationSession<ExtendedPart>::DeSerializationSession;
	virtual ExtendedPart* deserializeExtendedPart(std::istream& istream) override {
		Part p = ::deserializePartWithoutCFrame(istream, this->IDToShapeClassMap);
		Material mat = deserializeMaterial(istream);
		int renderMode = ::deserialize<int>(istream);
		ExtendedPart* result = new ExtendedPart(std::move(p), ::deserializeString(istream));

		result->material = mat;
		result->renderMode = renderMode;

		return result;
	}
};

void WorldImportExport::saveLooseParts(const char* fileName, size_t numberOfParts, const ExtendedPart* const* parts) {
	std::ofstream partFile;
	partFile.open(fileName, std::ios::binary);

	for(size_t i = 0; i < numberOfParts; i++) {
		const ExtendedPart* curPart = parts[i];
		
		throw "TODO";
		//::serializePartWithCFrame(*(parts[i]), partFile);
	}
	partFile.close();
}
void WorldImportExport::loadLoosePartsIntoWorld(const char* fileName, World<ExtendedPart>& world) {
	world.addExternalForce(new ExternalGravity(Vec3(0, -10.0, 0.0)));

	std::ifstream file;
	file.open(fileName, std::ios::binary);

	while(!file.eof()) {
		throw "TODO";
		//ExtendedPart* p = new ExtendedPart(::deserializePartWithCFrame(file));

		//world.addPart(p);
	}
	file.close();
}


void WorldImportExport::saveSingleMotorizedPhysical(const char* fileName, const MotorizedPhysical& physical) {
	std::ofstream physFile;
	physFile.open(fileName, std::ios::binary);

	throw "TODO";
	//serializeMotorizedPhysical(physical, physFile, partSerializer);
	physFile.close();
}
void WorldImportExport::loadSingleMotorizedPhysicalIntoWorld(const char* fileName, World<ExtendedPart>& world) {
	std::ifstream file;
	file.open(fileName, std::ios::binary);

	throw "TODO";
	//MotorizedPhysical* phys = ::deserializeMotorizedPhysical(file, partSerializer);
	file.close();
	//world.addPart(phys->getMainPart());
}


void WorldImportExport::saveWorld(const char* fileName, const World<ExtendedPart>& world) {
	std::ofstream file;
	file.open(fileName, std::ios::binary);

	Serializer serializer;
	serializer.serializeWorld(world, file);

	file.close();
}
void WorldImportExport::loadWorld(const char* fileName, World<ExtendedPart>& world) {
	std::ifstream file;
	file.open(fileName, std::ios::binary);

	Deserializer deserializer;
	deserializer.deserializeWorld(world, file);

	file.close();
}
};
