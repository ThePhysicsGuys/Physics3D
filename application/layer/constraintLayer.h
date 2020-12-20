#pragma once

#include "../engine/layer/layer.h"
#include "../graphics/visualData.h"

namespace P3D::Application {

class Screen;

class ConstraintLayer : public Engine::Layer {
public:
	VisualData hexagon;
	inline ConstraintLayer() {};
	inline ConstraintLayer(Screen* screen, char flags = None) : Layer("Constraint layer", screen, flags) {}

	virtual void onInit(Engine::Registry64& registry) override;
	virtual void onUpdate(Engine::Registry64& registry) override;
	virtual void onRender(Engine::Registry64& registry) override;
	virtual void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	virtual void onClose(Engine::Registry64& registry) override;
};
};
