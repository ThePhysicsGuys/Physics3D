#pragma once

#include "layer.h"

class Screen;

class PostprocessLayer : public Layer {
public:
	PostprocessLayer();
	PostprocessLayer(Screen* screen, char flags = noUpdate | noEvents);

	void init() override;
	void update() override;
	void render() override;
	void close() override;
};