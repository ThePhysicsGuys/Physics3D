#pragma once

#include <set>

#include "../graphics/texture.h"
#include "../graphics/gui/color.h"
#include "Physics3D/datastructures/smartPointers.h"
#include "Physics3D/math/mathUtil.h"

namespace P3D::Graphics::Comp {

// The mesh of an entity, as it is rendered
struct Mesh : public RC {
	typedef int Flags;

	inline static Flags Flags_None = 0 << 0;
	inline static Flags Flags_Normal = 1 << 0;
	inline static Flags Flags_UV = 1 << 1;
	inline static Flags Flags_Tangent = 1 << 2;
	inline static Flags Flags_Bitangent = 1 << 3;

	// The mesh id in the mesh registry
	std::size_t id;

	// Flags indicated the type of data this mesh contains
	Flags flags;

	// Whether the mesh is visible
	bool visible;

	Mesh()
		: id(-1)
		, flags(Flags_None)
		, visible(false) {}

	Mesh(std::size_t id, Flags flags = Flags_None)
		: id(id)
		, flags(flags)
		, visible(true) {}

	bool valid() const {
		return id != -1;
	}
};

struct Material : public RC {
	static constexpr std::size_t MAP_COUNT = 8;

private:
	SRef<Graphics::Texture> maps[MAP_COUNT];

public:
	typedef int Map;
	inline static Map Map_None = 0 << 0;
	inline static Map Map_Albedo = 1 << 0;
	inline static Map Map_Normal = 1 << 1;
	inline static Map Map_Metalness = 1 << 2;
	inline static Map Map_Roughness = 1 << 3;
	inline static Map Map_AO = 1 << 4;
	inline static Map Map_Gloss = 1 << 5;
	inline static Map Map_Specular = 1 << 6;
	inline static Map Map_Displacement = 1 << 7;

	Color albedo;
	float metalness;
	float roughness;
	float ao;

	constexpr Material(const Color& albedo = Color(1), float metalness = 0.0f, float roughness = 1.0f, float ao = 1.0f)
		: albedo(albedo)
		, metalness(metalness)
		, roughness(roughness)
		, ao(ao) {}

	~Material() override = default;

	void set(Map map, SRef<Graphics::Texture> texture) {
		maps[ctz(map)] = texture;
	}

	void reset(Map map) {
		if (map == Map_None)
			return;

		maps[ctz(map)] = nullptr;
	}

	[[nodiscard]] SRef<Graphics::Texture> get(Map map) const {
		if (map == Map_None)
			return nullptr;

		return maps[ctz(map)];
	}

	[[nodiscard]] SRef<Graphics::Texture> get(std::size_t index) const {
		if (index >= MAP_COUNT)
			return nullptr;

		return maps[index];
	}

	/*[[nodiscard]] std::map<Map, Graphics::Texture*> getTextures() const {
		std::map<Map, Graphics::Texture*> result;

		for (std::size_t index = 0; index < MAP_COUNT; index++)
			if (maps[index] != nullptr)
				result.emplace(1 << index, maps[index]);

		return result;
	}*/

	[[nodiscard]] int getTextureCount() const {
		return has(Map_Albedo) + has(Map_Normal) + has(Map_Metalness) + has(Map_Roughness) + has(Map_AO) + has(Map_Gloss) + has(Map_Specular) +
			has(Map_Displacement);
	}

	[[nodiscard]] bool has(Map map) const {
		if (map == Map_None)
			return false;

		return maps[ctz(map)] != nullptr;
	}

	[[nodiscard]] Map getMaps() const {
		return Map_None | (has(Map_Albedo) ? Map_Albedo : Map_None) | (has(Map_Normal) ? Map_Normal : Map_None) | (
			has(Map_Metalness) ? Map_Metalness : Map_None) | (has(Map_Roughness) ? Map_Roughness : Map_None) | (has(Map_AO) ? Map_AO : Map_None) | (
			has(Map_Gloss) ? Map_Gloss : Map_None) | (has(Map_Specular) ? Map_Specular : Map_None) | (has(Map_Displacement)
			? Map_Displacement
			: Map_None);
	}
};


}
