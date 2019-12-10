#pragma once

#include "../engine/layer/layer.h"

struct PropertiesFrame;
struct DebugFrame;
struct EnvironmentFrame;
struct ImageFrame;

class GuiLayer : public Layer {
public:
	PropertiesFrame* propertiesFrame;
	DebugFrame* debugFrame;
	EnvironmentFrame* environmentFrame;
	ImageFrame* originalRender;
	ImageFrame* blurRender;

	GuiLayer();
	GuiLayer(Screen* screen, char flags = None);

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};