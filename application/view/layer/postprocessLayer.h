#pragma once

#include "layer.h"

class Screen;

class PostprocessLayer : public Layer {
public:
	PostprocessLayer();
	PostprocessLayer(Screen* screen);

	void init() override;
	void update() override;
	void render() override;
	void close() override;
};