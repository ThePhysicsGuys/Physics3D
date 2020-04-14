#pragma once

#include "../graphics/gui/color.h"
#include "../graphics/texture.h"
#include <array>
namespace Graphics {
class Texture;
};

namespace Application {

struct Material {
private:
	Graphics::Texture* maps[8];

public:
	enum Map : char {
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

	inline Material(const Color& albedo = Color(1), float metalness = 1.0f, float roughness = 1.0f, float ao = 1.0f) : albedo(albedo), metalness(metalness), roughness(roughness), ao(ao) {};

	void reset(Map flag);
	void set(Map flag, Graphics::Texture* map);
	Graphics::Texture* get(Map map) const;
};

};