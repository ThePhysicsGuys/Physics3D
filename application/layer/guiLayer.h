#pragma once

#include "../engine/layer/layer.h"

namespace Application {
	
class Screen;

class GuiLayer : public Layer {
public:
	inline GuiLayer() : Layer() {};
	inline GuiLayer(Screen* screen, char flags = None) : Layer("Gui", screen, flags) {};

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;
};

};