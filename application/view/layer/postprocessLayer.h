#pragma once

#include "layer.h"

class Screen;

class PostprocessLayer : public Layer {
public:
	PostprocessLayer();
	PostprocessLayer(Screen* screen, char flags = NoUpdate | NoEvents);

	void init() override;
	void update() override;
	void event(Event& event) override;
	void render() override;
	void close() override;
};