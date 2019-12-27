#pragma once

#include "../engine/layer/layer.h"

namespace Application {
	
class Screen;

class GuiLayer : public Layer {
public:
	GuiLayer() : Layer() {};
	GuiLayer(Screen* screen, char flags = None) : Layer("Gui", screen, flags) {};

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};

};