#pragma once

#include "../engine/layer/layer.h"
#include "../util/valueCycle.h"
#include "../graphics/gui/color.h"
#include "../physics/math/linalg/vec.h"

namespace P3D::Application {

struct SkyboxCycle : public Util::ValueCycle<::Color3, Util::linear> {
public:
	inline SkyboxCycle() {}
	inline SkyboxCycle(const ::Color3& night, const ::Color3& day, const ::Color3& dusk, const ::Color3& dawn, float midnightEnd, float middayStart, float middayEnd) {
		float _midnightEnd = midnightEnd / 24.0f;
		float _middayStart = middayStart / 24.0f;
		float _middayEnd = middayEnd / 24.0f;
		
		addKeyframe(0.0f, night);
		addKeyframe(_midnightEnd, night);
		addKeyframe(_midnightEnd + (_middayStart - _midnightEnd) * 0.5f, dawn);
		addKeyframe(_middayStart, day);
		addKeyframe(_middayEnd, day);
		addKeyframe(_middayEnd + (1.0f - _middayEnd) * 0.5f, dusk);
		addKeyframe(1.0f, night);
	}
};

class Screen;

class SkyboxLayer : public Engine::Layer {
public:
	inline SkyboxLayer() : Layer() {};
	inline SkyboxLayer(Screen* screen, char flags = NoEvents) : Layer("Skybox", screen, flags) {};

	virtual void onInit(Engine::Registry64& registry) override;
	virtual void onUpdate(Engine::Registry64& registry) override;
	virtual void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	virtual void onRender(Engine::Registry64& registry) override;
	virtual void onClose(Engine::Registry64& registry) override;
};

};