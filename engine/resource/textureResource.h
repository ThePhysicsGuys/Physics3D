#pragma once

#include "resource.h"

#include "../graphics/texture.h"

class TextureResource;

class TextureAllocator : public ResourceAllocator<TextureResource> {
public:
	virtual TextureResource* load(const std::string& name, const std::string& path) override;
};

class TextureResource : public Resource, public Texture {
public:
	DEFINE_RESOURCE(Texture, "../res/textures/default/default.png");

	TextureResource(const std::string& path, Texture&& texture) : Resource(path, path), Texture(std::move(texture)) {
	
	}

	TextureResource(const std::string& name, const std::string& path, Texture&& texture) : Resource(name, path), Texture(std::move(texture)) {
	
	}

	virtual void close() override {
		Texture::close();
	};

	static TextureAllocator getAllocator() {
		return TextureAllocator();
	}
};

