#include "core.h"

#include "testLayer.h"
#include "view/screen.h"

#include "ecs/entity.h";
#include "ecs/components/model.h"
#include "ecs/components/transform.h"

#include "../graphics/shader/shaderLexer.h"

namespace Application {

void TestLayer::onInit() {
	Entity* entity = new Entity();
	TransformComp* t = new TransformComp(GlobalCFrame());
	Model* m = new Model();

	entity->addComponent(t);
	entity->addComponent(m);

	std::string code =
		R"(
			[common]
			#version 330 core
			[vertex]
			layout(location = 0) in vec2 vposition;
			layout(location = 1) in vec2 vuv;
			layout(location = 2) in vec4 vcolor;
			uniform mat4 projectionMatrix;
			out vec4 fcolor;out vec2 fuv;
			void main() {
				gl_Position = projectionMatrix * vec4(vposition, 0.0, 1.0);
				fcolor = vcolor;fuv = vuv;
			}
			[fragment]
			out vec4 outColor;
			in vec2 fuv;
			in vec4 fcolor;
			uniform sampler2D textureSampler;
			uniform bool textured = false;
			void main() {
				outColor = (textured) ? texture(textureSampler, fuv) * fcolor : fcolor;
			}
		)";

	auto result = Lexer::lex(code);
}

void TestLayer::onUpdate() {

}

void TestLayer::onEvent(Event& event) {

}

void TestLayer::onRender() {

}

void TestLayer::onClose() {

}

};