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

FixedSharedObjectSerializerDeserializer<Graphics::Texture*> textureSerializer{nullptr};

void WorldImportExport::registerTexture(Graphics::Texture* texture) {
	textureSerializer.registerObject(texture);
}

static void serializeMaterial(const Material& material, std::ostream& ostream) {
	textureSerializer.serialize(material.get(Material::ALBEDO), ostream);
	textureSerializer.serialize(material.get(Material::NORMAL), ostream);
	textureSerializer.serialize(material.get(Material::METALNESS), ostream);
	textureSerializer.serialize(material.get(Material::ROUGHNESS), ostream);
	textureSerializer.serialize(material.get(Material::AO), ostream);
	textureSerializer.serialize(material.get(Material::GLOSS), ostream);
	textureSerializer.serialize(material.get(Material::SPECULAR), ostream);
	textureSerializer.serialize(material.get(Material::DISPLACEMENT), ostream);
	::serialize<Color>(material.albedo, ostream);
	::serialize<float>(material.metalness, ostream);
	::serialize<float>(material.roughness, ostream);
	::serialize<float>(material.ao, ostream);
}

static Material deserializeMaterial(std::istream& istream) {
	Graphics::Texture* albedoMap = textureSerializer.deserialize(istream);
	Graphics::Texture* normalMap = textureSerializer.deserialize(istream);
	Graphics::Texture* metalnessMap = textureSerializer.deserialize(istream);
	Graphics::Texture* roughnessMap = textureSerializer.deserialize(istream);
	Graphics::Texture* aoMap = textureSerializer.deserialize(istream);
	Graphics::Texture* glossMap = textureSerializer.deserialize(istream);
	Graphics::Texture* specularMap = textureSerializer.deserialize(istream);
	Graphics::Texture* displacementrMap = textureSerializer.deserialize(istream);
	Color albedo = ::deserialize<Color>(istream);
	float metalness = ::deserialize<float>(istream);
	float roughness = ::deserialize<float>(istream);
	float ao = ::deserialize<float>(istream);

	Material material = Material(albedo, metalness, roughness, ao);
	material.set(Material::ALBEDO, albedoMap);
	material.set(Material::ALBEDO, normalMap);
	material.set(Material::ALBEDO, metalnessMap);
	material.set(Material::ALBEDO, roughnessMap);
	material.set(Material::ALBEDO, aoMap);
	material.set(Material::ALBEDO, glossMap);
	material.set(Material::ALBEDO, specularMap);
	material.set(Material::ALBEDO, displacementrMap);

	return material;
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
