#pragma once

#include "../graphics/gui/color.h"
#include "../graphics/texture.h"
#include <array>

namespace P3D::Graphics {
class Texture;
};

namespace P3D::Application::Comp {

struct Material : public RefCountable {
private:
	Graphics::Texture* maps[8];

public:
	enum Map : unsigned char {
		NONE = 0 << 0,
		ALBEDO = 1 << 0,
		NORMAL = 1 << 1,
		METALNESS = 1 << 2,
		ROUGHNESS = 1 << 3,
		AO = 1 << 4,
		GLOSS = 1 << 5,
		SPECULAR = 1 << 6,
		DISPLACEMENT = 1 << 7,
	};

	char flags = NONE;
	
	Color albedo;
	float metalness;
	float roughness;
	float ao;

	Material(const Color& albedo = Color::full(1), float metalness = 1.0f, float roughness = 1.0f, float ao = 1.0f);

	void reset(Map flag);
	void set(Map flag, Graphics::Texture* map);
	Graphics::Texture* get(Map map) const;
};

};