#include "core.h"

#include "shaders.h"
#include "texture.h"

#include "../util/resource/resourceManager.h"
#include "../graphics/renderer.h"

#include <sstream>

namespace P3D::Graphics {

namespace Shaders {
SRef<GuiShader> guiShader;
SRef<QuadShader> quadShader;

void onInit() {
	std::string guiShaderSource =
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
				fcolor = vcolor;
				fuv = vuv;
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

	std::string quadShaderSource =
		R"(
			[common]
			#version 330 core
			[vertex]
			layout(location = 0) 
			in vec4 vposition;
			uniform mat4 projectionMatrix;
			out vec2 fUV;
			void main() {
				gl_Position = projectionMatrix * vec4(vposition.xy, 0.0, 1.0);fUV = vposition.zw;
			}
			[fragment]
			out vec4 outColor;
			in vec2 fUV;
			uniform bool textured;
			uniform sampler2D textureSampler;
			uniform vec4 color = vec4(1, 1, 1, 1);
			void main() {
				outColor = (textured) ? texture(textureSampler, fUV) * color : color;
			}
		)";

	// GShader init
	guiShader = std::make_shared<GuiShader>(guiShaderSource);
	quadShader = std::make_shared<QuadShader>(quadShaderSource);

	ResourceManager::add(guiShader.get());
	ResourceManager::add(quadShader.get());
}

void onClose() {
	guiShader->close();
	quadShader->close();
}
}


// GuiShader

void GuiShader::init(const Mat4f& orthoMatrix) {
	bind();
	setUniform("projectionMatrix", orthoMatrix);
	setUniform("textureSampler", 0);
}

void GuiShader::setTextured(bool textured) {
	bind();
	setUniform("textured", textured);
}


// QuadShader

void QuadShader::updateProjection(const Mat4f& projectionMatrix) {
	bind();
	setUniform("projectionMatrix", projectionMatrix);
}

void QuadShader::updateColor(const Vec4& color) {
	bind();
	setUniform("textured", false);
	setUniform("color", color);
}

void QuadShader::updateTexture(GLID texture, GLID unit) {
	updateTexture(texture, unit, Vec4f::full(1.0f));
}

void QuadShader::updateTexture(GLID texture, GLID unit, const Vec4f& color) {
	bind();
	Renderer::activeTexture(unit);
	Renderer::bindTexture2D(texture);
	setUniform("textured", true);
	setUniform("textureSampler", unit);
	setUniform("color", color);
}

};