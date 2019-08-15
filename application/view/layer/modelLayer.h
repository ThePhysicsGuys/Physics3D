#pragma once

#include "layer.h"

class Screen;

class ModelLayer : public Layer {
public:
	ModelLayer();
	ModelLayer(Screen* screen, char flags = none);

	void init() override;
	void update() override;
	void render() override;
	void close() override;
};