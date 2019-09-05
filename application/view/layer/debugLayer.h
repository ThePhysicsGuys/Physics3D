#pragma once

#include "layer.h"

class ArrayMesh;
class PointMesh;
class VectorMesh;

class DebugLayer : public Layer {
private:
	VectorMesh* vectorMesh = nullptr;
	PointMesh* pointMesh = nullptr;
	ArrayMesh* originMesh = nullptr;

public:
	DebugLayer();
	DebugLayer(Screen* screen, char flags = NoUpdate | NoEvents);

	void init() override;
	void update() override;
	void event(Event& event) override;
	void render() override;
	void close() override;
}; 
