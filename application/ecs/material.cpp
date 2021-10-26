#include "core.h"

#include "material.h"

#include "../graphics/texture.h"
#include <Physics3D/math/mathUtil.h>

namespace P3D::Application::Comp {

Material::Material(const Graphics::Color& albedo, float metalness, float roughness, float ao) : albedo(albedo), metalness(metalness), roughness(roughness), ao(ao) {
	
}

void Material::set(Map map, SRef<Graphics::Texture> texture) {
	assert(powOf2(map));
	maps[ctz(map)] = texture;
	if (texture != nullptr)
		flags |= map;
	else
		flags &= ~map;
}

void Material::reset(Map map) {
	assert(powOf2(map));
	maps[ctz(map)] = nullptr;
	flags &= ~map;
}

SRef<Graphics::Texture> Material::get(Map map) const {
	assert(powOf2(map));

	return has(map) ? maps[ctz(map)] : nullptr;
}

bool Material::has(Map map) const {
	return (flags & static_cast<std::underlying_type_t<Map>>(map)) != 0;
}

};