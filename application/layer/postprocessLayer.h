#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class PostprocessLayer : public Engine::Layer {
private:
	Engine::Registry64::entity_type entity;
	
public:
	PostprocessLayer() : Layer() {}
	PostprocessLayer(Screen* screen, char flags = 0) : Layer("Postprocess", screen, flags) {};

	void onInit(Engine::Registry64& registry) override;
	void onUpdate(Engine::Registry64& registry) override;
	void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	void onRender(Engine::Registry64& registry) override;
	void onClose(Engine::Registry64& registry) override;
};

};