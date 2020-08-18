#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class ImGuiLayer : public Engine::Layer {
public:
	bool sceneHovered = false;

	inline ImGuiLayer() : Layer() {};
	inline ImGuiLayer(Screen* screen, char flags = None) : Layer("ImGui", screen, flags) {};

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Engine::Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;

	void begin();
	void end();
};

};