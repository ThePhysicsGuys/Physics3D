#include "core.h"

#include "textureResource.h"

#include "../view/texture.h"

TextureResource* TextureAllocator::load(std::string name, std::string path) {
	Texture* texture = Texture::load(path);

	if (texture)
		return new TextureResource(name, path, texture);
	else
		return nullptr;
}