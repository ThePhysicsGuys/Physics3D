#pragma once

#include "../engine/layer/layer.h"

namespace Application {

class Screen;

class TestLayer : public Layer {
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