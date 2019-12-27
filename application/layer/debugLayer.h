#pragma once

#include "../engine/layer/layer.h"

class ArrayMesh;
class PointMesh;
class VectorMesh;

namespace Application {

class DebugLayer : public Layer {
private:
	VectorMesh* vectorMesh = nullptr;
	PointMesh* pointMesh = nullptr;
	ArrayMesh* originMesh = nullptr;

public:
	DebugLayer();
	DebugLayer(Screen* screen, char flags = NoUpdate | NoEvents);

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};

};