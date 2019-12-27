#pragma once

#include "../engine/layer/layer.h"

namespace Application {

class Screen;

class TestLayer : public Layer {
public:
	TestLayer();
	TestLayer(Screen* screen, char flags = 0);

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};

};