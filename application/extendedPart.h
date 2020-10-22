#pragma once

#include "ecs/material.h"
#include "../engine/visualData.h"
#include "../graphics/visualShape.h"
#include "../physics/part.h"
#include "../engine/ecs/registry.h"

namespace P3D::Application {

struct ExtendedPart : public Part {
	using Entity = Engine::Registry64::entity_type;

public:
	Entity entity = 0;

	ExtendedPart() = default;
	ExtendedPart(Part&& part, const std::string& name = "", const Entity& parent = 0);
	ExtendedPart(Part&& part, const VisualData& visualData, const std::string& name = "", const Entity& parent = 0);
	ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, const std::string& name = "", const Entity& parent = 0);
	ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, const VisualData& visualData, const std::string& name = "", const Entity& parent = 0);
	ExtendedPart(const Shape& hitbox, ExtendedPart* attachTo, const CFrame& attach, const PartProperties& properties, const std::string& name = "", const Entity& parent = 0);

	void setMaterial(const Comp::Material& material);
	void setName(const std::string& name);
};

};