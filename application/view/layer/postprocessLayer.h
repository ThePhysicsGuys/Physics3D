#pragma once

#include "layer.h"

class Screen;

class PostprocessLayer : public Layer {
public:
	PostprocessLayer();
	PostprocessLayer(Screen* screen, char flags = NoUpdate | NoEvents);

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};