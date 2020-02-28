#pragma once

#include "../graphics/shader/shader.h"
#include "../graphics/resource/shaderResource.h"
#include "../graphics/gui/color.h"
#include "../physics/math/globalCFrame.h"

class HDRTexture;
class Texture;
class CubeMap;

namespace Application {

struct Light;
struct Material;
struct ExtendedPart;

struct SkyboxShader : public ShaderResource {
	SkyboxShader() : ShaderResource() {}
	SkyboxShader(ShaderSource shaderSource) : ShaderResource("skyboxShader", "skybox.shader", shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix);
	void updateCubeMap(CubeMap* skybox);
	void updateLightDirection(const Vec3f& lightDirection);
};

struct MaskShader : public ShaderResource {
	MaskShader() : ShaderResource() {}
	MaskShader(ShaderSource shaderSource) : ShaderResource("MaskShader", "mask.shader", shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix);
	void updateModel(const Mat4f& modelMatrix);
	void updateModel(const GlobalCFrame& modelCFrame, DiagonalMat3f scale);
	void updateColor(const Color& color);
};

struct BasicShader : public ShaderResource {
	BasicShader() : ShaderResource() {}
	BasicShader(ShaderSource shaderSource) : ShaderResource("BasicShader", "basic.shader", shaderSource) {}

	void updateSunDirection(const Vec3f& sunDirection);
	void updateSunColor(const Color3& sunColor);
	void updateGamma(float gamma);
	void updateHDR(bool hdr);
	void updateExposure(float exposure);
	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
	void updateLight(const std::vector<Light*> lights);
	void updatePart(const ExtendedPart& part);
	void updateIncludeNormalsAndUVs(bool includeNormals, bool includeUVs);
	void updateMaterial(const Material& material);
	void updateModel(const Mat4f& modelMatrix);
	void updateModel(const GlobalCFrame& modelCFrame, DiagonalMat3f scale);
	void updateUniforms(int id);
};

struct DepthShader : public ShaderResource {
	DepthShader() : ShaderResource() {}
	DepthShader(ShaderSource shaderSource) : ShaderResource("DepthShader", "depth.shader", shaderSource) {}

	void updateLight(const Mat4f& lightMatrix);
	void updateModel(const Mat4f& modelMatrix);
	void updateModel(const GlobalCFrame& modelCFrame, DiagonalMat3f scale);
};

struct PostProcessShader : public ShaderResource {
	PostProcessShader() : ShaderResource() {}
	PostProcessShader(ShaderSource shaderSource) : ShaderResource("PostProcessShader", "postprocess.shader", shaderSource) {}

	void updateTexture(Texture* texture);
	void updateTexture(HDRTexture* texture);
};

struct OriginShader : public ShaderResource {
	OriginShader() : ShaderResource() {}
	OriginShader(ShaderSource shaderSource) : ShaderResource("OriginShader", "origin.shader", shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& rotatedViewMatrix, const Mat4f& projectionMatrix, const Mat4f& orthoMatrix, const Position& viewPosition);
};

struct FontShader : public ShaderResource {
	FontShader() : ShaderResource() {}
	FontShader(ShaderSource shaderSource) : ShaderResource("FontShader", "font.shader", shaderSource) {}

	void updateColor(const Color& color);
	void updateProjection(const Mat4f& projectionMatrix);
	void updateTexture(Texture* texture);
};

struct VectorShader : public ShaderResource {
	VectorShader() : ShaderResource() {}
	VectorShader(ShaderSource shaderSource) : ShaderResource("VectorShader", "vector.shader", shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
};

struct PointShader : public ShaderResource {
	PointShader() : ShaderResource() {}
	PointShader(ShaderSource shaderSource) : ShaderResource("PointShader", "point.shader", shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
};

struct TestShader : public ShaderResource {
	TestShader() : ShaderResource() {}
	TestShader(ShaderSource shaderSource) : ShaderResource("TestShader", "test.shader", shaderSource) {}

	void updateProjection(const Mat4f& projectionMatrix);
	void updateView(const Mat4f& viewMatrix);
	void updateModel(const Mat4f& modelMatrix);
	void updateModel(const GlobalCFrame& modelCFrame, DiagonalMat3f scale);
	void updateViewPosition(const Position& viewPosition);
	void updateDisplacement(Texture* displacementMap);
};

struct LineShader : public ShaderResource {
	LineShader() : ShaderResource() {}
	LineShader(ShaderSource shaderSource) : ShaderResource("LineShader", "line.shader", shaderSource) {}

	void updateProjection(const Mat4f& projectionMatrix, const Mat4f& viewMatrix);
};

struct InstanceBasicShader : public ShaderResource {
	InstanceBasicShader() : ShaderResource() {}
	InstanceBasicShader(ShaderSource shaderSource) : ShaderResource("InstanceBasicShader", "instance_basic.shader", shaderSource) {}

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