#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class PickerLayer : public Engine::Layer {
public:
	inline PickerLayer() : Layer() {};
	inline PickerLayer(Screen* screen, char flags = None) : Layer("Picker", screen, flags) {};

	virtual void onInit(Engine::Registry64& registry) override;
	virtual void onUpdate(Engine::Registry64& registry) override;
	virtual void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	virtual void onRender(Engine::Registry64& registry) override;
	virtual void onClose(Engine::Registry64& registry) override;
};

};