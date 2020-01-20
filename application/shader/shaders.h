#pragma once

#include "../graphics/shader/shader.h"
#include "../graphics/gui/color.h"

class HDRTexture;
class Texture;
class CubeMap;
struct Light;
struct Material;

namespace Application {

struct ExtendedPart;

struct SkyboxShader : public Shader {
	SkyboxShader() : Shader() {}
	SkyboxShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix);
	void updateCubeMap(CubeMap* skybox);
	void updateLightDirection(const Vec3f& lightDirection);
};

struct MaskShader : public Shader {
	MaskShader() : Shader() {}
	MaskShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix);
	void updateModel(const Mat4f& modelMatrix);
	void updateColor(const Color& color);
};

struct BasicShader : public Shader {
	BasicShader() : Shader() {}
	BasicShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateSunDirection(const Vec3f& sunDirection);
	void updateSunColor(const Color3& sunColor);
	void updateGamma(float gamma);
	void updateHDR(bool hdr);
	void updateExposure(float exposure);
	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
	void updateLight(Light lights[], int size);
	void updatePart(const ExtendedPart& part);
	void updateMaterial(const Material& material);
	void updateModel(const Mat4f& modelMatrix);
	void updateUniforms(int id);
};

struct DepthShader : public Shader {
	DepthShader() : Shader() {}
	DepthShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateLight(const Mat4f& lightMatrix);
	void updateModel(const Mat4f& modelMatrix);
};

struct PostProcessShader : public Shader {
	PostProcessShader() : Shader() {}
	PostProcessShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateTexture(Texture* texture);
	void updateTexture(HDRTexture* texture);
};

struct OriginShader : public Shader {
	OriginShader() : Shader() {}
	OriginShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& rotatedViewMatrix, const Mat4f& projectionMatrix, const Mat4f& orthoMatrix, const Position& viewPosition);
};

struct FontShader : public Shader {
	FontShader() : Shader() {}
	FontShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateColor(const Color& color);
	void updateProjection(const Mat4f& projectionMatrix);
	void updateTexture(Texture* texture);
};

struct VectorShader : public Shader {
	VectorShader() : Shader() {}
	VectorShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
};

struct PointShader : public Shader {
	PointShader() : Shader() {}
	PointShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
};

struct TestShader : public Shader {
	TestShader() : Shader() {}
	TestShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateProjection(const Mat4f& projectionMatrix);
	void updateView(const Mat4f& viewMatrix);
	void updateModel(const Mat4f& modelMatrix);
	void updateViewPosition(const Position& viewPosition);
	void updateDisplacement(Texture* displacementMap);
};

struct LineShader : public Shader {
	LineShader() : Shader() {}
	LineShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateProjection(const Mat4f& projectionMatrix, const Mat4f& viewMatrix);
};

struct InstanceBasicShader : public Shader {
	InstanceBasicShader() : Shader() {}
	InstanceBasicShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateSunDirection(const Vec3f& sunDirection);
	void updateSunColor(const Vec3f& sunColor);
	void updateGamma(float gamma);
	void updateHDR(bool hdr);
	void updateExposure(float exposure);
	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
	void updateLight(Light lights[], int size);
};

namespace ApplicationShaders {
extern BasicShader basicShader;
extern DepthShader depthShader;
extern VectorShader vectorShader;
extern OriginShader originShader;
extern FontShader fontShader;
extern PostProcessShader postProcessShader;
extern SkyboxShader skyboxShader;
extern PointShader pointShader;
extern TestShader testShader;
extern LineShader lineShader;
extern InstanceBasicShader instanceBasicShader;
extern MaskShader maskShader;

void onInit();
void onClose();
}

};