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

struct OutlineShader : public ShaderResource {
	OutlineShader() : ShaderResource("OutlineShader", "../res/shaders/outline.shader") {}

	void updateColor(const Color& color);
	void updateUnit(GLID id);
};

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

	void updateTexture(SRef<Texture> texture);
	void updateTexture(SRef<HDRTexture> texture);
};

struct OriginShader : public ShaderResource {
	OriginShader() : ShaderResource("OriginShader", "../res/shaders/origin.shader") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& rotatedViewMatrix, const Mat4f& projectionMatrix, const Mat4f& orthoMatrix, const Position& viewPosition);
};

struct FontShader : public ShaderResource {
	FontShader() : ShaderResource("FontShader", "../res/shaders/font.shader") {}

	void updateColor(const Color& color);
	void updateProjection(const Mat4f& projectionMatrix);
	void updateTexture(SRef<Texture> texture);
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

struct BlurShader : public ShaderResource {
	BlurShader() : ShaderResource("BlurShader", "../res/shaders/blur.shader") {}

	void updateDirection(bool horizontal);
	void updateUnit(int unit);
};

namespace Shaders {
extern URef<BasicShader> basicShader;
extern URef<DepthShader> depthShader;
extern URef<VectorShader> vectorShader;
extern URef<OriginShader> originShader;
extern URef<FontShader> fontShader;
extern URef<PostProcessShader> postProcessShader;
extern URef<SkyboxShader> skyboxShader;
extern URef<PointShader> pointShader;
extern URef<TestShader> testShader;
extern URef<LineShader> lineShader;
extern URef<InstanceShader> instanceShader;
extern URef<MaskShader> maskShader;
extern URef<SkyShader> skyShader;
extern URef<LightingShader> lightingShader;
extern URef<DebugShader> debugShader;
extern URef<DepthBufferShader> depthBufferShader;
extern URef<OutlineShader> outlineShader;
extern URef<BlurShader> blurShader;
	
void onInit();
void onClose();
}

};