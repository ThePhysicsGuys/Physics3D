#include "core.h"

#include "shaders.h"
#include "resources.h"
#include "texture.h"

#include "../util/resourceLoader.h"

#include <sstream>

namespace GraphicsShaders {
	GuiShader guiShader;
	QuadShader quadShader;

	void onInit() {
		std::string guiShaderSourceFile =
			"[common]\n\r"
			"#version 330 core\n\r"
			"[vertex]\n\r"
			"layout(location = 0) in vec2 vposition;layout(location = 1) in vec2 vuv;layout(location = 2) in vec4 vcolor;uniform mat4 projectionMatrix;out vec4 fcolor;out vec2 fuv;void main() {gl_Position = projectionMatrix * vec4(vposition, 0.0, 1.0);fcolor = vcolor;fuv = vuv;}\n\r"
			"[fragment]\n\r"
			"out vec4 outColor;in vec2 fuv;in vec4 fcolor;uniform sampler2D textureSampler;uniform bool textured = false;void main() {outColor = (textured) ? texture(textureSampler, fuv) * fcolor : fcolor;}\n\r";

		std::string quadShaderSourceFile =
			"[common]\n\r"
			"#version 330 core\n\r"
			"[vertex]\n\r"
			"layout(location = 0) in vec4 vposition;uniform mat4 projectionMatrix;out vec2 fUV;void main() {gl_Position = projectionMatrix * vec4(vposition.xy, 0.0, 1.0);fUV = vposition.zw;}\n\r"
			"[fragment]\n\r"
			"out vec4 outColor;in vec2 fUV;uniform bool textured;uniform sampler2D textureSampler;uniform vec4 color = vec4(1, 1, 1, 1);void main() {outColor = (textured) ? texture(textureSampler, fUV) * color : color;}\n\r";

		// Shader source init
		ShaderSource guiShaderSource = parseShader((std::istream&) std::istringstream(guiShaderSourceFile), "gui.shader");
		ShaderSource quadShaderSource = parseShader((std::istream&) std::istringstream(quadShaderSourceFile), "quad.shader");
		//ShaderSource guiShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(graphicsResources, GUI_SHADER)), "gui.shader");
		//ShaderSource quadShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(graphicsResources, QUAD_SHADER)), "quad.shader");

		// Shader init
		new(&guiShader) GuiShader(guiShaderSource);
		new(&quadShader) QuadShader(quadShaderSource);
	}

	void onClose() {
		guiShader.close();
		quadShader.close();
	}
}


// GuiShader

void GuiShader::init(const Mat4f& orthoMatrix) {
	bind();
	shader.setUniform("projectionMatrix", orthoMatrix);
	shader.setUniform("textureSampler", 0);
}

void GuiShader::setTextured(bool textured) {
	bind();
	shader.setUniform("textured", textured);
}


// QuadShader

void QuadShader::updateProjection(const Mat4f& orthoMatrix) {
	bind();
	shader.setUniform("projectionMatrix", orthoMatrix);
}

void QuadShader::updateColor(const Vec4& color) {
	bind();
	shader.setUniform("textured", false);
	shader.setUniform("color", color);
}

void QuadShader::updateTexture(Texture* texture) {
	updateTexture(texture, Vec4f(1));
}

void QuadShader::updateTexture(Texture* texture, const Vec4f& color) {
	bind();
	texture->bind();
	shader.setUniform("textured", true);
	shader.setUniform("textureSampler", texture->getUnit());
	shader.setUniform("color", color);
}
