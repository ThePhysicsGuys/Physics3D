#include "core.h"

#include "textureResource.h"

TextureResource* TextureAllocator::load(const std::string& name, const std::string& path) {
	Graphics::Texture texture = Graphics::Texture::load(path);

	if (texture.getID() != 0) {
		return new TextureResource(name, path, std::move(texture));
	} else {
		return nullptr;
	}
}