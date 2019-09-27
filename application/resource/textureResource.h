#pragma once

#include "core.h"

#include "resourceManager.h"

class TextureResource;
class Texture;

class TextureAllocator : public ResourceAllocator<TextureResource> {
public:
	virtual TextureResource* load(std::string name, std::string path) override;
};

class TextureResource : public Resource {
private:
	Texture* texture = nullptr;

public:
	DEFINE_RESOURCE(Texture, "default/defaultTexture.png");

	TextureResource(std::string path, Texture* texture) : Resource(path), texture(texture) {
	
	}

	TextureResource(std::string name, std::string path, Texture* texture) : Resource(name, path), texture(texture) {
	
	}

	Texture* getTexture() {
		return texture;
	}

	static TextureAllocator getAllocator() {
		return TextureAllocator();
	}
};

