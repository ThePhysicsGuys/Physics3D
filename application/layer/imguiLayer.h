#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class ImGuiLayer : public Engine::Layer {
public:
	bool sceneHovered = false;

	inline ImGuiLayer() : Layer() {};
	inline ImGuiLayer(Screen* screen, char flags = None) : Layer("ImGui", screen, flags) {};

	virtual void onInit(Engine::Registry64& registry) override;
	virtual void onUpdate(Engine::Registry64& registry) override;
	virtual void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	virtual void onRender(Engine::Registry64& registry) override;
	virtual void onClose(Engine::Registry64& registry) override;

	void begin();
	void end();
};

};