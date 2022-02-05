#pragma once

#include "../engine/layer/layer.h"
#include "../util/valueCycle.h"
#include "../graphics/gui/color.h"

namespace P3D {
namespace Graphics {
	class CubeMap;
}
}

namespace P3D::Application {

struct SkyboxCycle : public Util::ValueCycle<Graphics::Color, Util::linear> {
public:
	SkyboxCycle() {}
	SkyboxCycle(const Graphics::Color& night, const Graphics::Color& day, const Graphics::Color& dusk, const Graphics::Color& dawn, float midnightEnd, float middayStart, float middayEnd) {
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

float getScroll(const Screen* screen);
extern bool useNewSky;
extern bool pauze;
extern float time;


class SkyboxLayer : public Engine::Layer {
public:
	static Graphics::CubeMap* skyboxTexture;

	SkyboxLayer() : Layer() {}
	SkyboxLayer(Screen* screen, char flags = NoEvents) : Layer("Skybox", screen, flags) {}

	void onInit(Engine::Registry64& registry) override;
	void onUpdate(Engine::Registry64& registry) override;
	void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	void onRender(Engine::Registry64& registry) override;
	void onClose(Engine::Registry64& registry) override;
};

};