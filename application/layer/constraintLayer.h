#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class ConstraintLayer : public Engine::Layer {
public:
	ConstraintLayer() = default;
	ConstraintLayer(Screen* screen, char flags = None) : Layer("Constraint layer", screen, flags) {}

	virtual ~ConstraintLayer() = default;

	void onInit(Engine::Registry64& registry) override;
	void onUpdate(Engine::Registry64& registry) override;
	void onRender(Engine::Registry64& registry) override;
	void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	void onClose(Engine::Registry64& registry) override;
};
};
