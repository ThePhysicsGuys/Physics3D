#include "core.h"
#include "GL/glew.h"

#include "testLayer.h"
#include "view/screen.h"
#include "../graphics/renderer.h"
#include "imgui/imgui.h"

namespace P3D::Application {

void TestLayer::onInit(Engine::Registry64& registry) {
	
}

void TestLayer::onUpdate(Engine::Registry64& registry) {

}

void TestLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {
	using namespace Engine;

}

void TestLayer::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace Renderer;

	Screen* screen = static_cast<Screen*>(this->ptr);
}

void TestLayer::onClose(Engine::Registry64& registry) {
}

};