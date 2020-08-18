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

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Engine::Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;
};

};