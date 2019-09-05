#pragma once

#include "layer.h"

class Screen;

class PickerLayer : public Layer {
public:
	PickerLayer();
	PickerLayer(Screen* screen, char flags = None);

	void init() override;
	void update() override;
	void event(Event& event) override;
	void render() override;
	void close() override;
};