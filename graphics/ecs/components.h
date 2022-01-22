#pragma once

#include "Physics3D/datastructures/smartPointers.h"

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

	bool valid() {
		return id != -1;
	}
};

}
