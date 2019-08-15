#pragma once

#include "layer.h"

class DebugOverlay : public Layer {
public:
	DebugOverlay();
	DebugOverlay(Screen* screen, char flags = noEvents);


	void init() override;
	void update() override;
	void render() override;
	void close() override;
};