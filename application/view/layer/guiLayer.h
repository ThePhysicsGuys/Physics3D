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

	void init() override;
	void update() override;
	void event(Event& event) override;
	void render() override;
	void close() override;
};