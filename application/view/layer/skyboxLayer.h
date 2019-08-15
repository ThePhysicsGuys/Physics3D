#pragma once

#include "layer.h"

class Screen;

class SkyboxLayer : public Layer {
public:
	SkyboxLayer();
	SkyboxLayer(Screen* screen, char flags = noUpdate | noEvents);

	void init() override;
	void update() override;
	void render() override;
	void close() override;
};