#pragma once

#include "ecs/material.h"
#include "../graphics/extendedTriangleMesh.h"
#include <Physics3D/part.h>
#include "../engine/ecs/registry.h"

namespace P3D::Application {

struct ExtendedPart : public Part {
	using Entity = Engine::Registry64::entity_type;

public:
	Entity entity = 0;

	ExtendedPart() = default;
	ExtendedPart(Part&& part, const std::string& name = "", const Entity& parent = 0);
	ExtendedPart(Part&& part, const Graphics::Comp::Mesh& mesh, const std::string& name = "", const Entity& parent = 0);
	ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, const std::string& name = "", const Entity& parent = 0);
	ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, const Graphics::Comp::Mesh& mesh, const std::string& name = "", const Entity& parent = 0);
	ExtendedPart(const Shape& hitbox, ExtendedPart* attachTo, const CFrame& attach, const PartProperties& properties, const std::string& name = "", const Entity& parent = 0);

	~ExtendedPart();

	void setMaterial(const Comp::Material& material);
	void setName(const std::string& name);

	void setColor(const Graphics::Color& color);
	Graphics::Color getColor() const;
};

};