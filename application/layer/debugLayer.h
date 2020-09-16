#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Graphics {
class ArrayMesh;
class PointMesh;
class VectorMesh;
};

class Screen;

namespace P3D::Application {

class DebugLayer : public Engine::Layer {
private:
	Graphics::VectorMesh* vectorMesh = nullptr;
	Graphics::PointMesh* pointMesh = nullptr;
	Graphics::ArrayMesh* originMesh = nullptr;

public:
	inline DebugLayer() : Layer() {};
	inline DebugLayer(Screen* screen, char flags = NoUpdate | NoEvents) : Layer("Debug layer", screen, flags) {};

	virtual void onInit(Engine::Registry64& registry) override;
	virtual void onUpdate(Engine::Registry64& registry) override;
	virtual void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	virtual void onRender(Engine::Registry64& registry) override;
	virtual void onClose(Engine::Registry64& registry) override;
};

};