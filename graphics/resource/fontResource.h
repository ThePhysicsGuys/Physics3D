#pragma once

#include "../util/resource/resource.h"

#include "../font.h"

class FontResource;

class FontAllocator : public ResourceAllocator<FontResource> {
public:
	virtual FontResource* load(const std::string& name, const std::string& path) override;
};

class FontResource : public Resource, public Font {
public:
	DEFINE_RESOURCE(Font, "../res/fonts/default/default.ttf");

	FontResource(const std::string& path, Font&& font) : Resource(path, path), Font(std::move(font)) {

	}

	FontResource(const std::string& name, const std::string& path, Font&& font) : Resource(name, path), Font(std::move(font)) {

	}

	virtual void close() override {
		Font::close();
	};

	static FontAllocator getAllocator() {
		return FontAllocator();
	}
};

