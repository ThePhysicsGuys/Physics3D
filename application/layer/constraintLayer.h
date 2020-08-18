#pragma once

#include "../engine/layer/layer.h"
#include "../engine/visualData.h"

namespace P3D::Application {

class Screen;

class ConstraintLayer : public Engine::Layer {
public:
	VisualData hexagon;
	inline ConstraintLayer() {};
	inline ConstraintLayer(Screen* screen, char flags = None) : Layer("Constraint layer", screen, flags) {}

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onRender() override;
	virtual void onClose() override;
	virtual void onEvent(Engine::Event& event) override;
};
};
