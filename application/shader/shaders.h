#pragma once

#include "../graphics/shader/shader.h"
#include "../graphics/resource/shaderResource.h"
#include "../graphics/gui/color.h"
#include "../physics/math/globalCFrame.h"

namespace Graphics {
class HDRTexture;
class Texture;
class CubeMap;
};

namespace Application {
using namespace Graphics;

struct Light;
struct Material;
struct ExtendedPart;

struct SkyboxShader : public ShaderResource {
	inline SkyboxShader() : ShaderResource() {}
	inline SkyboxShader(ShaderSource shaderSource) : ShaderResource("skyboxShader", "skybox.shader", shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix);
	void updateCubeMap(Graphics::CubeMap* skybox);
	void updateLightDirection(const Vec3f& lightDirection);
};

struct MaskShader : public ShaderResource {
	inline MaskShader() : ShaderResource() {}
	inline MaskShader(ShaderSource shaderSource) : ShaderResource("MaskShader", "mask.shader", shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix);
	void updateModel(const Mat4f& modelMatrix);
	void updateModel(const GlobalCFrame& modelCFrame, DiagonalMat3f scale);
	void updateColor(const Color& color);
};

struct BasicShader : public ShaderResource {
	inline BasicShader() : ShaderResource() {}
	inline BasicShader(ShaderSource shaderSource) : ShaderResource("BasicShader", "basic.shader", shaderSource) {}

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
	inline DepthShader() : ShaderResource() {}
	inline DepthShader(ShaderSource shaderSource) : ShaderResource("DepthShader", "depth.shader", shaderSource) {}

	void updateLight(const Mat4f& lightMatrix);
	void updateModel(const Mat4f& modelMatrix);
	void updateModel(const GlobalCFrame& modelCFrame, DiagonalMat3f scale);
};

struct PostProcessShader : public ShaderResource {
	inline PostProcessShader() : ShaderResource() {}
	inline PostProcessShader(ShaderSource shaderSource) : ShaderResource("PostProcessShader", "postprocess.shader", shaderSource) {}

	void updateTexture(Graphics::Texture* texture);
	void updateTexture(Graphics::HDRTexture* texture);
};

struct OriginShader : public ShaderResource {
	inline OriginShader() : ShaderResource() {}
	inline OriginShader(ShaderSource shaderSource) : ShaderResource("OriginShader", "origin.shader", shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& rotatedViewMatrix, const Mat4f& projectionMatrix, const Mat4f& orthoMatrix, const Position& viewPosition);
};

struct FontShader : public ShaderResource {
	inline FontShader() : ShaderResource() {}
	inline FontShader(ShaderSource shaderSource) : ShaderResource("FontShader", "font.shader", shaderSource) {}

	void updateColor(const Color& color);
	void updateProjection(const Mat4f& projectionMatrix);
	void updateTexture(Graphics::Texture* texture);
};

struct VectorShader : public ShaderResource {
	inline VectorShader() : ShaderResource() {}
	inline VectorShader(ShaderSource shaderSource) : ShaderResource("VectorShader", "vector.shader", shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
};

struct PointShader : public ShaderResource {
	inline PointShader() : ShaderResource() {}
	inline PointShader(ShaderSource shaderSource) : ShaderResource("PointShader", "point.shader", shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
};

struct TestShader : public ShaderResource {
	inline TestShader() : ShaderResource() {}
	inline TestShader(ShaderSource shaderSource) : ShaderResource("TestShader", "test.shader", shaderSource) {}

	void updateProjection(const Mat4f& projectionMatrix);
	void updateView(const Mat4f& viewMatrix);
	void updateModel(const Mat4f& modelMatrix);
	void updateModel(const GlobalCFrame& modelCFrame, DiagonalMat3f scale);
	void updateViewPosition(const Position& viewPosition);
	void updateDisplacement(Graphics::Texture* displacementMap);
};

struct LineShader : public ShaderResource {
	inline LineShader() : ShaderResource() {}
	inline LineShader(ShaderSource shaderSource) : ShaderResource("LineShader", "line.shader", shaderSource) {}

	void updateProjection(const Mat4f& projectionMatrix, const Mat4f& viewMatrix);
};

struct InstanceShader : public ShaderResource {
	inline InstanceShader() : ShaderResource() {}
	inline InstanceShader(ShaderSource shaderSource) : ShaderResource("InstanceBasicShader", "instance_basic.shader", shaderSource) {}

	void updateSunDirection(const Vec3f& sunDirection);
	void updateSunColor(const Vec3f& sunColor);
	void updateGamma(float gamma);
	void updateHDR(bool hdr);
	void updateExposure(float exposure);
	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
	void updateLight(Light lights[], int size);
};

struct SkyShader : public ShaderResource {
	inline SkyShader() : ShaderResource() {}
	inline SkyShader(ShaderSource shaderSource) : ShaderResource("SkyShader", "sky.shader", shaderSource) {}

	void updateCamera(const Vec3f& viewPosition, const Mat4f& projectionMatrix, const Mat4f& viewMatrix);
	void updateTime(float time);
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
extern InstanceShader instanceShader;
extern MaskShader maskShader;
extern SkyShader skyShader;

void onInit();
void onClose();
}

};