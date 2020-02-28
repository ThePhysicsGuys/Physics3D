#pragma once

#include "../engine/layer/layer.h"

class ArrayMesh;
class PointMesh;
class VectorMesh;
class Screen;

namespace Application {

class DebugLayer : public Layer {
private:
	VectorMesh* vectorMesh = nullptr;
	PointMesh* pointMesh = nullptr;
	ArrayMesh* originMesh = nullptr;

public:
	inline DebugLayer() : Layer() {};
	inline DebugLayer(Screen* screen, char flags = NoUpdate | NoEvents) : Layer("Debug layer", screen, flags) {};

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;
};

};