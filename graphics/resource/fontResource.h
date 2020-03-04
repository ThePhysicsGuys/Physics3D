#pragma once

#include "../util/resource/resource.h"

#include "../font.h"

class FontResource;

class FontAllocator : public ResourceAllocator<FontResource> {
public:
	virtual FontResource* load(const std::string& name, const std::string& path) override;
};

class FontResource : public Resource, public Graphics::Font {
public:
	DEFINE_RESOURCE(Font, "../res/fonts/default/default.ttf");

	FontResource(const std::string& path, Graphics::Font&& font) : Resource(path, path), Graphics::Font(std::move(font)) {

	}

	FontResource(const std::string& name, const std::string& path, Graphics::Font&& font) : Resource(name, path), Graphics::Font(std::move(font)) {

	}

	virtual void close() override {
		Graphics::Font::close();
	};

	static FontAllocator getAllocator() {
		return FontAllocator();
	}
};

