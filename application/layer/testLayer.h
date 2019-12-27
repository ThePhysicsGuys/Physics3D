#pragma once

#include "../engine/layer/layer.h"

namespace Application {

class Screen;

class TestLayer : public Layer {
public:
	TestLayer() : Layer() {};
	TestLayer(Screen* screen, char flags = None) : Layer("TestLayer", screen, flags) {};

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};

};