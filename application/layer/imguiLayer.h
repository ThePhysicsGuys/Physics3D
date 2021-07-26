#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class ImGuiLayer : public Engine::Layer {
public:
	bool sceneHovered = false;

	ImGuiLayer() : Layer() {};
	ImGuiLayer(Screen* screen, char flags = None) : Layer("ImGui", screen, flags) {};

	void onInit(Engine::Registry64& registry) override;
	void onUpdate(Engine::Registry64& registry) override;
	void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	void onRender(Engine::Registry64& registry) override;
	void onClose(Engine::Registry64& registry) override;

	void begin();
	void end();
};

};