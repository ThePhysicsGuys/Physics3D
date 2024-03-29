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
SRef<BlurShader> horizontalBlurShader;
SRef<BlurShader> verticalBlurShader;

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

	std::string horizontalBlurShaderVertexSource =
		R"(
			[common]
			#version 330
			[vertex]
			layout(location = 0) in vec2 vposition;
			out vec2 fUV[15];
			uniform float width = 3.0;
			void main() {
				float part = 1.0 / width;
				vec2 UV = vposition * 0.5 + 0.5;
				gl_Position = vec4(vposition, 0.0, 1.0);
				fUV[0]  = UV + vec2(0.0, -part * 7.0);
				fUV[1]  = UV + vec2(0.0, -part * 6.0);
				fUV[2]  = UV + vec2(0.0, -part * 5.0);
				fUV[3]  = UV + vec2(0.0, -part * 4.0);
				fUV[4]  = UV + vec2(0.0, -part * 3.0);
				fUV[5]  = UV + vec2(0.0, -part * 2.0);
				fUV[6]  = UV + vec2(0.0, -part);
				fUV[7]  = UV;
				fUV[8]  = UV + vec2(0.0,  part);
				fUV[9]  = UV + vec2(0.0,  part * 2.0);
				fUV[10] = UV + vec2(0.0,  part * 3.0);
				fUV[11] = UV + vec2(0.0,  part * 4.0);
				fUV[12] = UV + vec2(0.0,  part * 5.0);
				fUV[13] = UV + vec2(0.0,  part * 6.0);
				fUV[13] = UV + vec2(0.0,  part * 7.0);
			}
		)";

	std::string verticalBlurShaderVertexSource =
		R"(
			[common]
			#version 330
			[vertex]
			layout(location = 0) in vec2 vposition;
			out vec2 fUV[15];
			uniform float width = 3.0;
			void main() {
				float part = 1.0 / width;
				vec2 UV = vposition * 0.5 + 0.5;
				gl_Position = vec4(vposition, 0.0, 1.0);
				fUV[0]  = UV + vec2(-part * 7.0, 0.0);
				fUV[1]  = UV + vec2(-part * 6.0, 0.0);
				fUV[2]  = UV + vec2(-part * 5.0, 0.0);
				fUV[3]  = UV + vec2(-part * 4.0, 0.0);
				fUV[4]  = UV + vec2(-part * 3.0, 0.0);
				fUV[5]  = UV + vec2(-part * 2.0, 0.0);
				fUV[6]  = UV + vec2(-part, 0.0);
				fUV[7]  = UV;
				fUV[8]  = UV + vec2(part, 0.0);
				fUV[9]  = UV + vec2(part * 2.0, 0.0);
				fUV[10] = UV + vec2(part * 3.0, 0.0);
				fUV[11] = UV + vec2(part * 4.0, 0.0);
				fUV[12] = UV + vec2(part * 5.0, 0.0);
				fUV[13] = UV + vec2(part * 6.0, 0.0);
				fUV[14] = UV + vec2(part * 7.0, 0.0);
			}
		)";

	std::string blurShaderFragmentSource =
		R"(
			[fragment]
			in vec2 fUV[15];
			out vec4 outColor;
			uniform sampler2D image;
			void main() {
				outColor = vec4(0.0);
				outColor += texture(image, fUV[0]) * 0.0044299121055113265;
				outColor += texture(image, fUV[1]) * 0.00895781211794;
				outColor += texture(image, fUV[2]) * 0.0215963866053;
				outColor += texture(image, fUV[3]) * 0.0443683338718;
				outColor += texture(image, fUV[4]) * 0.0776744219933;
				outColor += texture(image, fUV[5]) * 0.115876621105;
				outColor += texture(image, fUV[6]) * 0.147308056121;
				outColor += texture(image, fUV[7]) * 0.159576912161;
				outColor += texture(image, fUV[8]) * 0.147308056121;
				outColor += texture(image, fUV[9]) * 0.115876621105;
				outColor += texture(image, fUV[10]) * 0.0776744219933;
				outColor += texture(image, fUV[11]) * 0.0443683338718;
				outColor += texture(image, fUV[12]) * 0.0215963866053;
				outColor += texture(image, fUV[13]) * 0.00895781211794;
				outColor += texture(image, fUV[14]) * 0.0044299121055113265;
			}
		)";

	// GShader source init
	std::string horizontalBlurShaderSource(horizontalBlurShaderVertexSource + blurShaderFragmentSource);
	std::string verticalBlurShaderSource(verticalBlurShaderVertexSource + blurShaderFragmentSource);

	// GShader init
	guiShader = std::make_shared<GuiShader>(guiShaderSource);
	quadShader = std::make_shared<QuadShader>(quadShaderSource);
	horizontalBlurShader = std::make_shared<BlurShader>(horizontalBlurShaderSource);
	verticalBlurShader = std::make_shared<BlurShader>(verticalBlurShaderSource);

	ResourceManager::add(guiShader.get());
	ResourceManager::add(quadShader.get());
	ResourceManager::add(horizontalBlurShader.get());
	ResourceManager::add(verticalBlurShader.get());
}

void onClose() {
	horizontalBlurShader->close();
	verticalBlurShader->close();
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


// BlurShader
void BlurShader::updateWidth(float width) {
	bind();
	setUniform("width", width);
}

void BlurShader::updateTexture(SRef<Texture> texture) {
	bind();
	texture->bind();
	setUniform("image", texture->getUnit());
}

};