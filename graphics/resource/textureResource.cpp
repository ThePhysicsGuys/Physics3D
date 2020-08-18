#include "core.h"

#include "textureResource.h"

namespace P3D::Graphics {

TextureResource* TextureAllocator::load(const std::string& name, const std::string& path) {
	Texture texture = Texture::load(path);

	if (texture.getID() != 0) {
		return new TextureResource(name, path, std::move(texture));
	} else {
		return nullptr;
	}
}

};