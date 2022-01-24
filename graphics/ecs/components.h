#pragma once

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
private:
	SRef<Graphics::Texture> maps[8];

public:
	typedef short Map;
	inline static Map Map_None         = 0 << 0;
	inline static Map Map_Albedo       = 1 << 0;
	inline static Map Map_Normal       = 1 << 1;
	inline static Map Map_Metalness    = 1 << 2;
	inline static Map Map_Roughness    = 1 << 3;
	inline static Map Map_AO           = 1 << 4;
	inline static Map Map_Gloss        = 1 << 5;
	inline static Map Map_Specular     = 1 << 6;
	inline static Map Map_Displacement = 1 << 7;

	Color albedo;
	float metalness;
	float roughness;
	float ao;

	Material(const Color& albedo = Color(1), float metalness = 1.0f, float roughness = 1.0f, float ao = 1.0f) : albedo(albedo), metalness(metalness), roughness(roughness), ao(ao) {}

	void set(Map map, SRef<Graphics::Texture> texture) {
		maps[ctz(map)] = texture;
	}

	void reset(Map map) {
		maps[ctz(map)] = nullptr;
	}

	[[nodiscard]] SRef<Graphics::Texture> get(Map map) const {
		return maps[ctz(map)];
	}

	[[nodiscard]] bool has(Map map) const {
		return maps[ctz(map)] != nullptr;
	}
};


}
