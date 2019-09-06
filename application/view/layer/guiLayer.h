#pragma once

#include "layer.h"

struct PropertiesFrame;
struct DebugFrame;
struct EnvironmentFrame;

class GuiLayer : public Layer {
public:
	PropertiesFrame* propertiesFrame;
	DebugFrame* debugFrame;
	EnvironmentFrame* environmentFrame;

	GuiLayer();
	GuiLayer(Screen* screen, char flags = None);

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};