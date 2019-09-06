#pragma once

#include "layer.h"

class DebugOverlay : public Layer {
public:
	DebugOverlay();
	DebugOverlay(Screen* screen, char flags = NoEvents);

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};