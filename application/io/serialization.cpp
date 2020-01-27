#include "core.h"

#include "serialization.h"

#include "extendedPart.h"
#include "../physics/world.h"
#include "../worlds.h"

#include "../physics/misc/gravityForce.h"
#include "../physics/misc/serialization.h"

#include <fstream>

namespace Application {


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

class ApplicationSerializationContext : public SerializationContext<ExtendedPart> {
	virtual void serializeExtendedPart(const ExtendedPart& part, std::ostream& ostream, PartSerializationInformation& serializationInformation) const override {
		::serializePartWithoutCFrame(part, ostream, serializationInformation);
	}
	virtual ExtendedPart* deserializeExtendedPart(std::istream& istream, PartDeSerializationInformation& deserializationInformation) const override {
		return new ExtendedPart(::deserializePartWithoutCFrame(istream, deserializationInformation));
	}
} static context;


void WorldImportExport::saveWorld(const char* fileName, const World<ExtendedPart>& world) {
	std::ofstream file;
	file.open(fileName, std::ios::binary);

	context.serializeWorld(world, file);

	file.close();
}
void WorldImportExport::loadWorld(const char* fileName, World<ExtendedPart>& world) {
	std::ifstream file;
	file.open(fileName, std::ios::binary);

	context.deserializeWorld(world, file);

	file.close();
}
};