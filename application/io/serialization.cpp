#include "core.h"

#include "serialization.h"

#include "extendedPart.h"
#include "../physics/world.h"
#include "../worlds.h"

#include "../physics/misc/gravityForce.h"
#include "../physics/misc/serialization.h"

#include <fstream>

namespace Application {

class CustomSerializer : public ExtendedPartSerializer<ExtendedPart> {
	virtual void serialize(const ExtendedPart& part, std::ostream& ostream) override {
		::serializePart(part, ostream);
	}
	virtual ExtendedPart* deserialize(std::istream& istream) override {
		return new ExtendedPart(::deserializePart(istream));
	}
};
static CustomSerializer partSerializer;

void WorldImportExport::saveLooseParts(const char* fileName, size_t numberOfParts, const ExtendedPart* const* parts) {
	std::ofstream partFile;
	partFile.open(fileName, std::ios::binary);

	for(size_t i = 0; i < numberOfParts; i++) {
		const ExtendedPart* curPart = parts[i];
		::serialize<GlobalCFrame>(curPart->getCFrame(), partFile);
		partSerializer.serializePart(**parts, partFile);
	}
	partFile.close();
}
void WorldImportExport::loadLoosePartsIntoWorld(const char* fileName, World<ExtendedPart>& world) {
	world.addExternalForce(new ExternalGravity(Vec3(0, -10.0, 0.0)));

	std::ifstream file;
	file.open(fileName, std::ios::binary);

	while(!file.eof()) {
		ExtendedPart* p = new ExtendedPart(::deserializePart(file));

		world.addPart(p);
	}
	file.close();
}


void WorldImportExport::saveSingleMotorizedPhysical(const char* fileName, const MotorizedPhysical& physical) {
	std::ofstream physFile;
	physFile.open(fileName, std::ios::binary);

	serializeMotorizedPhysical(physical, physFile, partSerializer);
	physFile.close();
}
void WorldImportExport::loadSingleMotorizedPhysicalIntoWorld(const char* fileName, World<ExtendedPart>& world) {
	std::ifstream file;
	file.open(fileName, std::ios::binary);

	MotorizedPhysical* phys = ::deserializeMotorizedPhysical(file, partSerializer);
	file.close();
	world.addPart(phys->getMainPart());
}


void WorldImportExport::saveWorld(const char* fileName, const World<ExtendedPart>& world) {
	std::ofstream file;
	file.open(fileName, std::ios::binary);

	::serializeWorld(world, file, partSerializer);

	file.close();
}
void WorldImportExport::loadWorld(const char* fileName, World<ExtendedPart>& world) {
	std::ifstream file;
	file.open(fileName, std::ios::binary);

	::deserializeWorld(world, file, partSerializer);

	file.close();
}
};