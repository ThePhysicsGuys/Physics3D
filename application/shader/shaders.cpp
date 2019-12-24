#include "core.h"

#include "shaders.h"
#include "resources.h"
#include "extendedPart.h"

#include "../graphics/texture.h"
#include "../graphics/material.h"
#include "../graphics/light.h"

#include "../util/resource/resourceLoader.h"

#include <sstream>

namespace ApplicationShaders {
	BasicShader basicShader;
	DepthShader depthShader;
	VectorShader vectorShader;
	OriginShader originShader;
	FontShader fontShader;
	PostProcessShader postProcessShader;
	SkyboxShader skyboxShader;
	PointShader pointShader;
	TestShader testShader;
	LineShader lineShader;
	MaskShader maskShader;
	InstanceBasicShader instanceBasicShader;

	void onInit() {
		// Shader source init
		ShaderSource basicShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(applicationResources, BASIC_SHADER)), "basic.shader");
		ShaderSource depthShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(applicationResources, DEPTH_SHADER)), "depth.shader");
		ShaderSource vectorShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(applicationResources, VECTOR_SHADER)), "vector.shader");
		ShaderSource fontShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(applicationResources, FONT_SHADER)), "font.shader");
		ShaderSource originShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(applicationResources, ORIGIN_SHADER)), "origin.shader");
		ShaderSource postProcessShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(applicationResources, POSTPROCESS_SHADER)), "postProcess.shader");
		ShaderSource skyboxShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(applicationResources, SKYBOX_SHADER)), "skybox.shader");
		ShaderSource pointShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(applicationResources, POINT_SHADER)), "point.shader");
		ShaderSource testShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(applicationResources, TEST_SHADER)), "test.shader");
		ShaderSource lineShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(applicationResources, LINE_SHADER)), "line.shader");
		ShaderSource maskShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(applicationResources, MASK_SHADER)), "mask.shader");
		ShaderSource instanceBasicShaderSource = parseShader((std::istream&) std::istringstream(getResourceAsString(applicationResources, INSTANCE_BASIC_SHADER)), "instance_basic.shader");

		// Shader init
		new(&basicShader) BasicShader(basicShaderSource);
		new(&depthShader) DepthShader(depthShaderSource);
		new(&vectorShader) VectorShader(vectorShaderSource);
		new(&fontShader) FontShader(fontShaderSource);
		new(&originShader) OriginShader(originShaderSource);
		new(&postProcessShader) PostProcessShader(postProcessShaderSource);
		new(&skyboxShader) SkyboxShader(skyboxShaderSource);
		new(&pointShader) PointShader(pointShaderSource);
		new(&testShader) TestShader(testShaderSource);
		new(&lineShader) LineShader(lineShaderSource);
		new(&maskShader) MaskShader(maskShaderSource);
		new(&instanceBasicShader) InstanceBasicShader(instanceBasicShaderSource);
	}

	void onClose() {
		basicShader.close();
		depthShader.close();
		vectorShader.close();
		fontShader.close();
		originShader.close();
		skyboxShader.close();
		postProcessShader.close();
		pointShader.close();
		testShader.close();
		lineShader.close();
		maskShader.close();
		instanceBasicShader.close();
	}
}


// BasicShader

void BasicShader::updateUniforms(int id) {
	bind();
	shader.setUniform("uniforms", id);
}

void BasicShader::updatePart(const ExtendedPart& part) {
	bind();
	shader.setUniform("includeNormals", int(part.visualData.includeNormals));
	shader.setUniform("includeUvs", int(part.visualData.includeUVs));
	shader.setUniform("modelMatrix", TransformToMat4(part.getPosition(), Mat3f(part.getCFrame().getRotation()) * DiagonalMat3f(part.hitbox.scale)));
}

void BasicShader::updateMaterial(const Material& material) {
	bind();
	shader.setUniform("material.ambient", material.ambient);
	shader.setUniform("material.diffuse", material.diffuse);
	shader.setUniform("material.specular", material.specular);
	shader.setUniform("material.reflectance", material.reflectance);

	if (material.texture) {
		material.texture->bind();
		shader.setUniform("material.textured", 1);
		shader.setUniform("textureSampler", material.texture->getUnit());
	} else {
		shader.setUniform("material.textured", 0);
		shader.setUniform("textureSampler", 0);
	}

	if (material.normal) {
		material.normal->bind();
		shader.setUniform("material.normalmapped", 1);
		shader.setUniform("normalSampler", material.normal->getUnit());
	} else {
		shader.setUniform("material.normalmapped", 0);
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

void BasicShader::updateSunColor(const Color3& sunColor) {
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

void BasicShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition) {
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

void SkyboxShader::updateCubeMap(CubeMap* skybox) {
	bind();
	shader.setUniform("skyboxTexture", skybox->getUnit());
}

void SkyboxShader::updateLightDirection(const Vec3f& lightDirection) {
	bind();
	shader.setUniform("lightDirection", lightDirection);
}


// MeskShader

void MaskShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
	shader.setUniform("projectionMatrix", projectionMatrix);
}

void MaskShader::updateModel(const Mat4f& modelMatrix) {
	bind();
	shader.setUniform("modelMatrix", modelMatrix);
}

void MaskShader::updateColor(const Color& color) {
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


// PostProcessShader

void PostProcessShader::updateTexture(Texture* texture) {
	bind();
	texture->bind();
	shader.setUniform("textureSampler", texture->getUnit());
}

void PostProcessShader::updateTexture(HDRTexture* texture) {
	bind();
	texture->bind();
	shader.setUniform("textureSampler", texture->getUnit());
}


// OriginShader

void OriginShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& rotatedViewMatrix, const Mat4f& projectionMatrix, const Mat4f& orthoMatrix, const Position& viewPosition) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
	shader.setUniform("rotatedViewMatrix", rotatedViewMatrix);
	shader.setUniform("projectionMatrix", projectionMatrix);
	shader.setUniform("orthoMatrix", orthoMatrix);
	shader.setUniform("viewPosition", viewPosition);
}


// FontShader

void FontShader::updateColor(const Color& color) {
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
	shader.setUniform("text", texture->getUnit());
}


// VectorShader

void VectorShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
	shader.setUniform("projectionMatrix", projectionMatrix);
	shader.setUniform("viewPosition", viewPosition);
}


// PointShader

void PointShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition) {
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

void TestShader::updateViewPosition(const Position& viewPosition) {
	bind();
	shader.setUniform("viewPosition", viewPosition);
}

void TestShader::updateDisplacement(Texture* displacementMap) {
	bind();
	displacementMap->bind();
	shader.setUniform("displacementMap", displacementMap->getUnit());
}


// LineShader

void LineShader::updateProjection(const Mat4f& projectionMatrix, const Mat4f& viewMatrix) {
	bind();
	shader.setUniform("projectionMatrix", projectionMatrix);
	shader.setUniform("viewMatrix", viewMatrix);
}


// InstanceBasicShader

void InstanceBasicShader::updateLight(Light lights[], int size) {
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

void InstanceBasicShader::createLightArray(int size) {
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

void InstanceBasicShader::updateSunDirection(const Vec3f& sunDirection) {
	bind();
	shader.setUniform("sunDirection", sunDirection);
}

void InstanceBasicShader::updateSunColor(const Vec3f& sunColor) {
	bind();
	shader.setUniform("sunColor", sunColor);
}

void InstanceBasicShader::updateGamma(float gamma) {
	bind();
	shader.setUniform("gamma", gamma);
}

void InstanceBasicShader::updateHDR(bool hdr) {
	bind();
	shader.setUniform("hdr", hdr);
}

void InstanceBasicShader::updateExposure(float exposure) {
	bind();
	shader.setUniform("exposure", exposure);
}

void InstanceBasicShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
	shader.setUniform("projectionMatrix", projectionMatrix);
	shader.setUniform("viewPosition", viewPosition);
}