#pragma once

#include "layer.h"

class DebugOverlay : public Layer {
public:
	DebugOverlay();
	DebugOverlay(Screen* screen);


	void init() override;
	void update() override;
	void render() override;
	void close() override;
};