#include "core.h"

#include "serialization.h"

#include "../util/fileUtils.h"

#include "extendedPart.h"
#include "application.h"
#include "view/screen.h"
#include "ecs/components.h"
#include "../engine/ecs/registry.h"
#include <Physics3D/world.h>
#include "../worlds.h"

#include <Physics3D/externalforces/directionalGravity.h>
#include <Physics3D/misc/toString.h>
#include <Physics3D/misc/serialization/serialization.h>

#include <fstream>
#include <sstream>

namespace P3D::Application {

FixedSharedObjectSerializerDeserializer<Graphics::Texture*> textureSerializer{nullptr};

void WorldImportExport::registerTexture(Graphics::Texture* texture) {
	textureSerializer.registerObject(texture);
}

static void serializeMaterial(const Comp::Material& material, std::ostream& ostream) {
	textureSerializer.serialize(material.get(Comp::Material::ALBEDO).get(), ostream);
	textureSerializer.serialize(material.get(Comp::Material::NORMAL).get(), ostream);
	textureSerializer.serialize(material.get(Comp::Material::METALNESS).get(), ostream);
	textureSerializer.serialize(material.get(Comp::Material::ROUGHNESS).get(), ostream);
	textureSerializer.serialize(material.get(Comp::Material::AO).get(), ostream);
	textureSerializer.serialize(material.get(Comp::Material::GLOSS).get(), ostream);
	textureSerializer.serialize(material.get(Comp::Material::SPECULAR).get(), ostream);
	textureSerializer.serialize(material.get(Comp::Material::DISPLACEMENT).get(), ostream);
	serializeBasicTypes<Graphics::Color>(material.albedo, ostream);
	serializeBasicTypes<float>(material.metalness, ostream);
	serializeBasicTypes<float>(material.roughness, ostream);
	serializeBasicTypes<float>(material.ao, ostream);
}

static Comp::Material deserializeMaterial(std::istream& istream) {
	SRef<Graphics::Texture> albedoMap(textureSerializer.deserialize(istream));
	SRef<Graphics::Texture> normalMap(textureSerializer.deserialize(istream));
	SRef<Graphics::Texture> metalnessMap(textureSerializer.deserialize(istream));
	SRef<Graphics::Texture> roughnessMap(textureSerializer.deserialize(istream));
	SRef<Graphics::Texture> aoMap(textureSerializer.deserialize(istream));
	SRef<Graphics::Texture> glossMap(textureSerializer.deserialize(istream));
	SRef<Graphics::Texture> specularMap(textureSerializer.deserialize(istream));
	SRef<Graphics::Texture> displacementrMap(textureSerializer.deserialize(istream));
	Graphics::Color albedo = deserializeBasicTypes<Graphics::Color>(istream);
	float metalness = deserializeBasicTypes<float>(istream);
	float roughness = deserializeBasicTypes<float>(istream);
	float ao = deserializeBasicTypes<float>(istream);

	Comp::Material material = Comp::Material(albedo, metalness, roughness, ao);
	material.set(Comp::Material::ALBEDO, albedoMap);
	material.set(Comp::Material::ALBEDO, normalMap);
	material.set(Comp::Material::ALBEDO, metalnessMap);
	material.set(Comp::Material::ALBEDO, roughnessMap);
	material.set(Comp::Material::ALBEDO, aoMap);
	material.set(Comp::Material::ALBEDO, glossMap);
	material.set(Comp::Material::ALBEDO, specularMap);
	material.set(Comp::Material::ALBEDO, displacementrMap);

	return material;
}

class Serializer : public SerializationSession<ExtendedPart> {
public:
	using SerializationSession<ExtendedPart>::SerializationSession;
	virtual void serializePartExternalData(const ExtendedPart& part, std::ostream& ostream) override {
		// TODO integrate components into serialization
		serializeMaterial(screen.registry.getOr<Comp::Material>(part.entity), ostream);
		serializeString(screen.registry.getOr<Comp::Name>(part.entity, "").name, ostream);
	}
};

class Deserializer : public DeSerializationSession<ExtendedPart> {
public:
	using DeSerializationSession<ExtendedPart>::DeSerializationSession;
	virtual ExtendedPart* deserializeExtendedPart(Part&& partPhysicalData, std::istream& istream) override {
		Comp::Material material = deserializeMaterial(istream);
		ExtendedPart* result = new ExtendedPart(std::move(partPhysicalData), deserializeString(istream));

		result->setMaterial(material);

		return result;
	}
};

static void openWriteFile(std::ofstream& fstream, const char* fileName) {
	std::string fullPath = Util::getFullPath(fileName);
	Log::info("Writing to file %s", fullPath.c_str());
	fstream.open(fullPath, std::ios::binary);
	if(!fstream.is_open()) {
		throw "File not opened!";
	}
}

static void openReadFile(std::ifstream& fstream, const char* fileName) {
	std::string fullPath = Util::getFullPath(fileName);
	Log::info("Reading file %s", fullPath.c_str());
	fstream.open(fullPath, std::ios::binary);
	if(!fstream.is_open()) {
		throw "File not opened!";
	}
}

void WorldImportExport::saveLooseParts(const char* fileName, size_t numberOfParts, const ExtendedPart* const parts[]) {
	std::ofstream file;
	openWriteFile(file, fileName);

	Serializer serializer;
	serializer.serializeParts(parts, numberOfParts, file);

	file.close();
}
void WorldImportExport::loadLoosePartsIntoWorld(const char* fileName, World<ExtendedPart>& world) {
	std::ifstream file;
	openReadFile(file, fileName);

	Deserializer d;
	std::vector<ExtendedPart*> result = d.deserializeParts(file);
	file.close();

	for(ExtendedPart* p : result) {
		world.addPart(p);
	}
}

void WorldImportExport::loadNativePartsIntoWorld(const char* fileName, World<ExtendedPart>& world) {
	std::ifstream file;
	openReadFile(file, fileName);

	DeSerializationSessionPrototype d;
	std::vector<Part*> result = d.deserializeParts(file);
	file.close();

	for(Part* p : result) {
		Log::debug("Part cframe: %s", str(p->getCFrame()).c_str());
		world.addPart(new ExtendedPart(std::move(*p)));
	}
}

void WorldImportExport::saveWorld(const char* fileName, const World<ExtendedPart>& world) {
	std::ofstream file;
	openWriteFile(file, fileName);

	Serializer serializer;
	serializer.serializeWorld(world, file);

	file.close();
}
void WorldImportExport::loadWorld(const char* fileName, World<ExtendedPart>& world) {
	std::ifstream file;
	openReadFile(file, fileName);

	if(!file.is_open()) {
		throw std::runtime_error("Could not open file!");
	}

	Deserializer deserializer;
	deserializer.deserializeWorld(world, file);

	assert(world.isValid());

	file.close();
}
};
