#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class PostprocessLayer : public Engine::Layer {
public:
	inline PostprocessLayer() : Layer() {};
	inline PostprocessLayer(Screen* screen, char flags = NoUpdate | NoEvents) : Layer("Postprocess", screen, flags) {};

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Engine::Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;
};

};