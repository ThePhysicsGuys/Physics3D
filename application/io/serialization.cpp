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

FixedSharedObjectSerializerDeserializer<Texture*> textureSerializer{nullptr};

void WorldImportExport::registerTexture(Texture* texture) {
	textureSerializer.registerObject(texture);
}

static void serializeMaterial(const Material& material, std::ostream& ostream) {
	textureSerializer.serialize(material.texture, ostream);
	textureSerializer.serialize(material.normal, ostream);
	::serialize<Color>(material.ambient, ostream);
	::serialize<Color3>(material.diffuse, ostream);
	::serialize<Color3>(material.specular, ostream);
	::serialize<float>(material.reflectance, ostream);
}

static Material deserializeMaterial(std::istream& istream) {
	Texture* texture = textureSerializer.deserialize(istream);
	Texture* normal = textureSerializer.deserialize(istream);
	Color ambient = ::deserialize<Color>(istream);
	Color3 diffuse = ::deserialize<Color3>(istream);
	Color3 specular = ::deserialize<Color3>(istream);
	float reflectance = ::deserialize<float>(istream);

	return Material(ambient, diffuse, specular, reflectance, texture, normal);
}

class Serializer : public SerializationSession<ExtendedPart> {
public:
	using SerializationSession<ExtendedPart>::SerializationSession;
	virtual void serializeExtendedPart(const ExtendedPart& part, std::ostream& ostream) override {
		serializeMaterial(part.material, ostream);
		::serialize<int>(part.renderMode, ostream);
		::serializeString(part.name, ostream);
	}
};

class Deserializer : public DeSerializationSession<ExtendedPart> {
public:
	using DeSerializationSession<ExtendedPart>::DeSerializationSession;
	virtual ExtendedPart* deserializeExtendedPart(Part&& partPhysicalData, std::istream& istream) override {
		Material mat = deserializeMaterial(istream);
		int renderMode = ::deserialize<int>(istream);
		ExtendedPart* result = new ExtendedPart(std::move(partPhysicalData), ::deserializeString(istream));

		result->material = mat;
		result->renderMode = renderMode;

		return result;
	}
};

void WorldImportExport::saveLooseParts(const char* fileName, size_t numberOfParts, const ExtendedPart* const parts[]) {
	std::ofstream partFile;
	partFile.open(fileName, std::ios::binary);

	Serializer serializer;
	serializer.serializeParts(parts, numberOfParts, partFile);

	partFile.close();
}
void WorldImportExport::loadLoosePartsIntoWorld(const char* fileName, World<ExtendedPart>& world) {
	std::ifstream file;
	file.open(fileName, std::ios::binary);

	Deserializer d;
	std::vector<ExtendedPart*> result = d.deserializeParts(file);
	file.close();

	for(ExtendedPart* p : result) {
		world.addPart(p);
	}
}

void WorldImportExport::loadNativePartsIntoWorld(const char* fileName, World<ExtendedPart>& world) {
	std::ifstream file;
	file.open(fileName, std::ios::binary);

	DeSerializationSessionPrototype d;
	std::vector<Part*> result = d.deserializeParts(file);
	file.close();

	for(Part* p : result) {
		world.addPart(new ExtendedPart(std::move(*p)));
	}
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
