#pragma once

#include "layer.h"

class Screen;

class ModelLayer : public Layer {
public:
	ModelLayer();
	ModelLayer(Screen* screen, char flags = None);

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};