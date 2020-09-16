#pragma once

#include "../engine/layer/layer.h"
#include "../graphics/shader/cshader.h"

namespace P3D::Application {

class Screen;

class TestLayer : public Engine::Layer {
private:
	Graphics::CShader shader;

public:
	inline TestLayer() : Layer() {};
	inline TestLayer(Screen* screen, char flags = None) : Layer("TestLayer", screen, flags) {};

	virtual void onInit(Engine::Registry64& registry) override;
	virtual void onUpdate(Engine::Registry64& registry) override;
	virtual void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	virtual void onRender(Engine::Registry64& registry) override;
	virtual void onClose(Engine::Registry64& registry) override;
};

};