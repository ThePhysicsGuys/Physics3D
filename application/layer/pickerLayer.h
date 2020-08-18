#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class PickerLayer : public Engine::Layer {
public:
	inline PickerLayer() : Layer() {};
	inline PickerLayer(Screen* screen, char flags = None) : Layer("Picker", screen, flags) {};

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Engine::Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;
};

};