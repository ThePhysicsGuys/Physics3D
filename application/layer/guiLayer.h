#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {
	
class Screen;

class GuiLayer : public Engine::Layer {
public:
	inline GuiLayer() : Layer() {};
	inline GuiLayer(Screen* screen, char flags = None) : Layer("Gui", screen, flags) {};

	virtual void onInit(Engine::Registry64& registry) override;
	virtual void onUpdate(Engine::Registry64& registry) override;
	virtual void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	virtual void onRender(Engine::Registry64& registry) override;
	virtual void onClose(Engine::Registry64& registry) override;
};

};