#pragma once

#include "../engine/tool/toolManager.h"
#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class PickerLayer : public Engine::Layer {
public:
	static std::vector<Engine::ToolManager> toolManagers;

	PickerLayer() : Layer() {}
	PickerLayer(Screen* screen, char flags = None) : Layer("Picker", screen, flags) {}

	void onInit(Engine::Registry64& registry) override;
	void onUpdate(Engine::Registry64& registry) override;
	void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	void onRender(Engine::Registry64& registry) override;
	void onClose(Engine::Registry64& registry) override;
};

};