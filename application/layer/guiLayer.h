#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {
	
class Screen;

class GuiLayer : public Engine::Layer {
public:
	GuiLayer() : Layer() {}
	GuiLayer(Screen* screen, char flags = None) : Layer("Gui", screen, flags) {}

	void onInit(Engine::Registry64& registry) override;
	void onUpdate(Engine::Registry64& registry) override;
	void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	void onRender(Engine::Registry64& registry) override;
	void onClose(Engine::Registry64& registry) override;
};

};