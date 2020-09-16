#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class PostprocessLayer : public Engine::Layer {
public:
	inline PostprocessLayer() : Layer() {};
	inline PostprocessLayer(Screen* screen, char flags = NoUpdate | NoEvents) : Layer("Postprocess", screen, flags) {};

	virtual void onInit(Engine::Registry64& registry) override;
	virtual void onUpdate(Engine::Registry64& registry) override;
	virtual void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	virtual void onRender(Engine::Registry64& registry) override;
	virtual void onClose(Engine::Registry64& registry) override;
};

};