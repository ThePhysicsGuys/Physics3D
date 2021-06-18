#pragma once

#include "../engine/layer/layer.h"
#include "../graphics/shader/shader.h"

namespace P3D::Application {

class Screen;

class TestLayer : public Engine::Layer {
private:
	SRef<Graphics::CShader> shader;

public:
	TestLayer() : Layer() {};
	TestLayer(Screen* screen, char flags = None) : Layer("TestLayer", screen, flags) {};

	void onInit(Engine::Registry64& registry) override;
	void onUpdate(Engine::Registry64& registry) override;
	void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	void onRender(Engine::Registry64& registry) override;
	void onClose(Engine::Registry64& registry) override;
};

};