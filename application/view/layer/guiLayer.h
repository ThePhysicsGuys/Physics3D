#pragma once

#include "layer.h"

// Temp
#include "../gui/frames.h"

class GuiLayer : public Layer {
public:
	PropertiesFrame* propertiesFrame;
	DebugFrame* debugFrame;
	EnvironmentFrame* environmentFrame;


	GuiLayer();
	GuiLayer(Screen* screen);

	void init() override;
	void update() override;
	void render() override;
	void close() override;
};