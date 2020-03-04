#include "core.h"

#include "fontResource.h"

FontResource* FontAllocator::load(const std::string& name, const std::string& path) {
	Graphics::Font font(path);

	if (font.getAtlasID() != 0) {
		return new FontResource(name, path, std::move(font));
	} else {
		return nullptr;
	}
}