#pragma once

#include "../engine/layer/layer.h"

class GuiLayer : public Layer {
public:
	GuiLayer();
	GuiLayer(Screen* screen, char flags = None);

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};