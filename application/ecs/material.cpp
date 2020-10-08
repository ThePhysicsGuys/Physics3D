#include "core.h"

#include "material.h"

#include "../graphics/texture.h"
#include "../physics/math/mathUtil.h"

#include <type_traits>

namespace P3D::Application::Comp {

Material::Material(const Color& albedo, float metalness, float roughness, float ao) : albedo(albedo), metalness(metalness), roughness(roughness), ao(ao) {
	
}

void Material::set(Map map, Graphics::Texture* texture) {
	assert(powOf2(map));
	maps[ctz(static_cast<std::underlying_type_t<Map>>(map))] = texture;
	bset(flags, map);
}

void Material::reset(Map map) {
	assert(powOf2(map));
	maps[ctz(static_cast<std::underlying_type_t<Map>>(map))] = nullptr;
	bclear(flags, map);
}

Graphics::Texture* Material::get(Map map) const {
	assert(powOf2(map));

	return (flags & map) ? maps[ctz(static_cast<std::underlying_type_t<Map>>(map))] : nullptr;
}

};