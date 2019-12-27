#pragma once

#include "../engine/layer/layer.h"

namespace Application {

class Screen;

class PickerLayer : public Layer {
public:
	PickerLayer() : Layer() {};
	PickerLayer(Screen* screen, char flags = None) : Layer("Picker", screen, flags) {};

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};

};