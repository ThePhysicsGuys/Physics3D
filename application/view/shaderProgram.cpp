#include "shaderProgram.h"

#include "texture.h"
#include "material.h"
#include "light.h"

#include "../resourceManager.h"
#include "../extendedPart.h"

#include <sstream>
#include <vector>

namespace Shaders {
	BasicShader basicShader;
	DepthShader depthShader;
	VectorShader vectorShader;
	OriginShader originShader;
	FontShader fontShader;
	QuadShader quadShader;
	PostProcessShader postProcessShader;
	SkyboxShader skyboxShader;
	PointShader pointShader;
	TestShader testShader;
	BlurShader blurShader;
	ColorWheelShader colorWheelShader;
	LineShader lineShader;

	void init() {
		// Shader source init
		ShaderSource basicShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BASIC_SHADER)), "basic.shader");
		ShaderSource depthShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(DEPTH_SHADER)), "depth.shader");
		ShaderSource vectorShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(VECTOR_SHADER)), "vector.shader");
		ShaderSource fontShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(FONT_SHADER)), "font.shader");
		ShaderSource originShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(ORIGIN_SHADER)), "origin.shader");
		ShaderSource quadShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(QUAD_SHADER)), "quad.shader");
		ShaderSource postProcessShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(POSTPROCESS_SHADER)), "postProcess.shader");
		ShaderSource skyboxShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(SKYBOX_SHADER)), "skybox.shader");
		ShaderSource pointShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(POINT_SHADER)), "point.shader");
		ShaderSource testShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(TEST_SHADER)), "test.shader");
		ShaderSource blurShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(BLUR_SHADER)), "blur.shader");
		ShaderSource colorWheelShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(COLORWHEEL_SHADER)), "colorwheel.shader");
		ShaderSource lineShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(LINE_SHADER)), "line.shader");

		// Shader init
		new(&basicShader) BasicShader(basicShaderSource);
		new(&depthShader) DepthShader(depthShaderSource);
		new(&vectorShader) VectorShader(vectorShaderSource);
		new(&fontShader) FontShader(fontShaderSource);
		new(&originShader) OriginShader(originShaderSource);
		new(&quadShader) QuadShader(quadShaderSource);
		new(&postProcessShader) PostProcessShader(postProcessShaderSource);
		new(&skyboxShader) SkyboxShader(skyboxShaderSource);
		new(&pointShader) PointShader(pointShaderSource);
		new(&testShader) TestShader(testShaderSource);
		new(&blurShader) BlurShader(blurShaderSource);
		new(&colorWheelShader) ColorWheelShader(colorWheelShaderSource);
		new(&lineShader) LineShader(lineShaderSource);

	}

	void close() {
		basicShader.close();
		depthShader.close();
		vectorShader.close();
		fontShader.close();
		originShader.close();
		skyboxShader.close();
		quadShader.close();
		postProcessShader.close();
		depthShader.close();
		skyboxShader.close();
		pointShader.close();
		testShader.close();
		blurShader.close();
		colorWheelShader.close();
		lineShader.close();
	}
}

template <class ...Args>
ShaderProgram::ShaderProgram(ShaderSource shaderSource, const Args&... args) : shaderSource(shaderSource), shader(shaderSource) {

	std::vector<std::string> uniforms = { args... };

	Log::setSubject(shader.name);

	for (const std::string& uniform : uniforms) {
		shader.createUniform(uniform);
	}

	Log::resetSubject();
}

void ShaderProgram::bind() {
	shader.bind();
}

void ShaderProgram::close() {
	shader.close();
}

ShaderProgram::~ShaderProgram() {
	close();
}


// BasicShader

void BasicShader::updatePart(const ExtendedPart& part) {
	bind();
	shader.setUniform("includeNormals", part.visualShape.normals != nullptr);
	shader.setUniform("includeUvs", part.visualShape.uvs != nullptr);
	shader.setUniform("modelMatrix", CFrameToMat4(CFrame(part.cframe)));
}

void BasicShader::updateMaterial(const Material& material) {
	bind();
	shader.setUniform("material.ambient", material.ambient);
	shader.setUniform("material.diffuse", material.diffuse);
	shader.setUniform("material.specular", material.specular);
	shader.setUniform("material.reflectance", material.reflectance);

	if (material.texture) {
		material.texture->bind();
		shader.setUniform("material.textured", true);
		shader.setUniform("textureSampler", material.texture->unit);
	} else {
		shader.setUniform("material.textured", false);
		shader.setUniform("textureSampler", 0);
	}

	if (material.normal) {
		material.normal->bind();
		shader.setUniform("material.normalmapped", true);
		shader.setUniform("normalSampler", material.normal->unit);
	} else {
		shader.setUniform("material.normalmapped", false);
		shader.setUniform("normalSampler", 0);
	}
}

void BasicShader::updateLight(Light lights[], int size) {
	bind();
	for (int i = 0; i < size; i++) {
		std::string uniform;
		std::string index = std::to_string(i);

		// position
		uniform = "lights[" + index + "].position";
		shader.setUniform(uniform, lights[i].position);

		// color
		uniform = "lights[" + index + "].color";
		shader.setUniform(uniform, lights[i].color);

		// intensity
		uniform = "lights[" + index + "].intensity";
		shader.setUniform(uniform, lights[i].intensity);

		// attenuation.constant
		uniform = "lights[" + index + "].attenuation.constant";
		shader.setUniform(uniform, lights[i].attenuation.constant);

		// attenuation.linear
		uniform = "lights[" + index + "].attenuation.linear";
		shader.setUniform(uniform, lights[i].attenuation.linear);

		// attenuation.exponent
		uniform = "lights[" + index + "].attenuation.exponent";
		shader.setUniform(uniform, lights[i].attenuation.exponent);
	}
}

void BasicShader::createLightArray(int size) {
	bind();
	for (int i = 0; i < size; i++) {
		std::string uniform;
		std::string index = std::to_string(i);

		// position
		uniform = "lights[" + index + "].position";
		shader.createUniform(uniform);

		// color
		uniform = "lights[" + index + "].color";
		shader.createUniform(uniform);

		// intensity
		uniform = "lights[" + index + "].intensity";
		shader.createUniform(uniform);

		// attenuation.constant
		uniform = "lights[" + index + "].attenuation.constant";
		shader.createUniform(uniform);

		// attenuation.linear
		uniform = "lights[" + index + "].attenuation.linear";
		shader.createUniform(uniform);

		// attenuation.exponent
		uniform = "lights[" + index + "].attenuation.exponent";
		shader.createUniform(uniform);
	}
}

void BasicShader::updateSunDirection(const Vec3f& sunDirection) {
	bind();
	shader.setUniform("sunDirection", sunDirection);
}

void BasicShader::updateSunColor(const Vec3f& sunColor) {
	bind();
	shader.setUniform("sunColor", sunColor);
}

void BasicShader::updateGamma(float gamma) {
	bind();
	shader.setUniform("gamma", gamma);
}

void BasicShader::updateHDR(bool hdr) {
	bind();
	shader.setUniform("hdr", hdr);
}

void BasicShader::updateExposure(float exposure) {
	bind();
	shader.setUniform("exposure", exposure);
}

void BasicShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Vec3f& viewPosition) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
	shader.setUniform("projectionMatrix", projectionMatrix);
	shader.setUniform("viewPosition", viewPosition);
}

void BasicShader::updateModel(const Mat4f& modelMatrix) {
	bind();
	shader.setUniform("modelMatrix", modelMatrix);
}


// SkyboxShader

void SkyboxShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
	shader.setUniform("projectionMatrix", projectionMatrix);
}

void SkyboxShader::updateCubeMap(const CubeMap& skybox) {
	bind();
	shader.setUniform("skyboxTexture", skybox.unit);
}

void SkyboxShader::updateLightDirection(const Vec3f& lightDirection) {
	bind();
	shader.setUniform("lightDirection", lightDirection);
}


// LineShader

void LineShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
	shader.setUniform("projectionMatrix", projectionMatrix);
}

void LineShader::updateModel(const Mat4f& modelMatrix) {
	bind();
	shader.setUniform("modelMatrix", modelMatrix);
}

void LineShader::updateColor(const Vec4f& color) {
	bind();
	shader.setUniform("color", color);
}


// DepthShader

void DepthShader::updateLight(const Mat4f& lightMatrix) {
	bind();
	shader.setUniform("lightMatrix", lightMatrix);
}

void DepthShader::updateModel(const Mat4f& modelMatrix) {
	bind();
	shader.setUniform("modelMatrix", modelMatrix);
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
	shader.setUniform("textureSampler", texture->unit);
	shader.setUniform("color", color);
}


// BlurShader

void BlurShader::updateType(BlurType type) {
	bind();
	shader.setUniform("horizontal", (int)type);
}

void BlurShader::updateTexture(Texture* texture) {
	bind();
	texture->bind();
	shader.setUniform("image", texture->unit);
}


// PostProcessShader

void PostProcessShader::updateTexture(Texture* texture) {
	bind();
	texture->bind();
	shader.setUniform("textureSampler", texture->unit);
}

void PostProcessShader::updateTexture(HDRTexture* texture) {
	bind();
	texture->bind();
	shader.setUniform("textureSampler", texture->unit);
}


// OriginShader

void OriginShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& rotatedViewMatrix, const Mat4f& projectionMatrix, const Mat4f& orthoMatrix, const Vec3f& viewPosition) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
	shader.setUniform("rotatedViewMatrix", rotatedViewMatrix);
	shader.setUniform("projectionMatrix", projectionMatrix);
	shader.setUniform("orthoMatrix", orthoMatrix);
	shader.setUniform("viewPosition", viewPosition);
}


// FontShader

void FontShader::updateColor(const Vec4f& color) {
	bind();
	shader.setUniform("color", color);
}

void FontShader::updateProjection(const Mat4f& projectionMatrix) {
	bind();
	shader.setUniform("projectionMatrix", projectionMatrix);
}

void FontShader::updateTexture(Texture* texture) {
	bind();
	texture->bind();
	shader.setUniform("text", texture->unit);
}


// VectorShader

void VectorShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Vec3f& viewPosition) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
	shader.setUniform("projectionMatrix", projectionMatrix);
	shader.setUniform("viewPosition", viewPosition);
}


// PointShader

void PointShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Vec3f& viewPosition) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
	shader.setUniform("projectionMatrix", projectionMatrix);
	shader.setUniform("viewPosition", viewPosition);
}


// TestShader

void TestShader::updateProjection(const Mat4f& projectionMatrix) {
	bind();
	shader.setUniform("projectionMatrix", projectionMatrix);
}

void TestShader::updateView(const Mat4f& viewMatrix) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
}

void TestShader::updateModel(const Mat4f& modelMatrix) {
	bind();
	shader.setUniform("modelMatrix", modelMatrix);
}

void TestShader::updateViewPosition(const Vec3f& viewPosition) {
	bind();
	shader.setUniform("viewPosition", viewPosition);
}

void TestShader::updateDisplacement(Texture* displacementMap) {
	bind();
	displacementMap->bind();
	shader.setUniform("displacementMap", displacementMap->unit);
}


// ColorWheelShader

void ColorWheelShader::updateProjection(Mat4f projectionMatrix) {
	bind();
	shader.setUniform("projectionMatrix", projectionMatrix);
}
