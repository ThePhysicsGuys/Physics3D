#pragma once

#include "../engine/layer/layer.h"

class ArrayMesh;
class PointMesh;
class VectorMesh;
class Screen;

class DebugLayer : public Layer {
private:
	VectorMesh* vectorMesh = nullptr;
	PointMesh* pointMesh = nullptr;
	ArrayMesh* originMesh = nullptr;

public:
	DebugLayer() : Layer() {};
	DebugLayer(Screen* screen, char flags = NoUpdate | NoEvents) : Layer("Debug layer", screen, flags) {};

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
}; 
