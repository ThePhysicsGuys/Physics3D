#pragma once

#include "../graphics/gui/color.h"
#include "../graphics/texture.h"

namespace P3D::Graphics {
class Texture;
};

namespace P3D::Application::Comp {

struct Material : public RC {
private:
	Graphics::Texture* maps[8];

public:
	enum Map : short {
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

	short flags = NONE;

	Graphics::Color albedo;
	float metalness;
	float roughness;
	float ao;

	Material(const Graphics::Color& albedo = Graphics::Color(1), float metalness = 1.0f, float roughness = 1.0f, float ao = 1.0f);

	void reset(Map flag);
	void set(Map flag, Graphics::Texture* map);
	
	[[nodiscard]] Graphics::Texture* get(Map map) const;
	[[nodiscard]] bool has(Map map) const;
};

};