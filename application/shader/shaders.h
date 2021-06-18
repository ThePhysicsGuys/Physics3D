#pragma once

#include <Physics3D/math/globalCFrame.h>
#include "../graphics/shader/shader.h"
#include "../graphics/resource/shaderResource.h"
#include "../graphics/gui/color.h"
#include "../shader/basicShader.h"
#include "../shader/shaderBase.h"

namespace P3D::Graphics {
	class HDRTexture;
	class Texture;
	class CubeMap;
};

namespace P3D::Application {
using namespace Graphics;

struct Light;
struct Material;
struct ExtendedPart;

struct DepthBufferShader : public ShaderResource {
	DepthBufferShader() : ShaderResource("DepthBufferShader", "../res/shaders/depthbuffer.shader") {}

	void updateDepthMap(GLID unit, GLID id);
	void updatePlanes(float near, float far);
};

struct DebugShader : public StandardMeshShaderBase {
	DebugShader() : ShaderResource("DebugShader", "../res/shaders/debug.shader"), StandardMeshShaderBase("DebugShader", "../res/shaders/debug.shader") {}
};

struct SkyboxShader : public ProjectionShaderBase {
	SkyboxShader() : ShaderResource("SkyboxShader", "../res/shaders/skybox.shader"), ProjectionShaderBase("SkyboxShader", "../res/shaders/skybox.shader") {}

	void updateCubeMap(CubeMap* skybox);
	void updateLightDirection(const Vec3f& lightDirection);
};

struct MaskShader : public StandardMeshShaderBase {
	MaskShader() : ShaderResource("MaskShader", "../res/shaders/mask.shader"), StandardMeshShaderBase("MaskShader", "../res/shaders/mask.shader") {}

	void updateColor(const Color& color);
};

struct DepthShader : public StandardMeshShaderBase {
	DepthShader() : ShaderResource("DepthShader", "../res/shaders/depth.shader"), StandardMeshShaderBase("DepthShader", "../res/shaders/depth.shader") {}

	void updateLight(const Mat4f& lightMatrix);
};

struct PostProcessShader : public ShaderResource {
	 PostProcessShader() : ShaderResource("PostProcessShader", "../res/shaders/postProcess.shader") {}

	void updateTexture(Texture* texture);
	void updateTexture(HDRTexture* texture);
};

struct OriginShader : public ShaderResource {
	OriginShader() : ShaderResource("OriginShader", "../res/shaders/origin.shader") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& rotatedViewMatrix, const Mat4f& projectionMatrix, const Mat4f& orthoMatrix, const Position& viewPosition);
};

struct FontShader : public ShaderResource {
	FontShader() : ShaderResource("FontShader", "../res/shaders/font.shader") {}

	void updateColor(const Color& color);
	void updateProjection(const Mat4f& projectionMatrix);
	void updateTexture(Texture* texture);
};

struct VectorShader : public ProjectionShaderBase {
	VectorShader() : ShaderResource("VectorShader", "../res/shaders/vector.shader"), ProjectionShaderBase("VectorShader", "../res/shaders/vector.shader") {}
};

struct PointShader : public ProjectionShaderBase {
	PointShader() : ShaderResource("PointShader", "../res/shaders/point.shader"), ProjectionShaderBase("PointShader", "../res/shaders/point.shader") {}
};

struct TestShader : public StandardMeshShaderBase {
	TestShader() : ShaderResource("TestShader", "../res/shaders/test.shader"), StandardMeshShaderBase("TestShader", "../res/shaders/test.shader") {}
};

struct LineShader : public ProjectionShaderBase {
	LineShader() : ShaderResource("LineShader", "../res/shaders/line.shader"), ProjectionShaderBase("LineShader", "../res/shaders/line.shader") {}
};

struct SkyShader : public ProjectionShaderBase {
	SkyShader() : ShaderResource("SkyShader", "../res/shaders/sky.shader"), ProjectionShaderBase("SkyShader", "../res/shaders/sky.shader") {}

	void updateTime(float time);
};

struct LightingShader : public BasicShaderBase {
	LightingShader() : ShaderResource("LightingShader", "../res/shaders/lighting.shader"), BasicShaderBase("LightingShader", "../res/shaders/lighting.shader") {}
};

namespace Shaders {
extern SRef<BasicShader> basicShader;
extern SRef<DepthShader> depthShader;
extern SRef<VectorShader> vectorShader;
extern SRef<OriginShader> originShader;
extern SRef<FontShader> fontShader;
extern SRef<PostProcessShader> postProcessShader;
extern SRef<SkyboxShader> skyboxShader;
extern SRef<PointShader> pointShader;
extern SRef<TestShader> testShader;
extern SRef<LineShader> lineShader;
extern SRef<InstanceShader> instanceShader;
extern SRef<MaskShader> maskShader;
extern SRef<SkyShader> skyShader;
extern SRef<LightingShader> lightingShader;
extern SRef<DebugShader> debugShader;
extern SRef<DepthBufferShader> depthBufferShader;

void onInit();
void onClose();
}

};