#pragma once

#include "../graphics/shader/shader.h"
#include "../graphics/resource/shaderResource.h"
#include "../graphics/gui/color.h"
#include "../physics/math/globalCFrame.h"
#include "../shader/basicShader.h"
#include "../shader/shaderBase.h"

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

struct DebugShader : public StandardMeshShaderBase {
	inline DebugShader() : ShaderResource() {}
	inline DebugShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "debug.shader", shaderSource) {}
};

struct SkyboxShader : public ProjectionShaderBase {
	inline SkyboxShader() : ShaderResource() {}
	inline SkyboxShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "skybox.shader", shaderSource) {}

	void updateCubeMap(Graphics::CubeMap* skybox);
	void updateLightDirection(const Vec3f& lightDirection);
};

struct MaskShader : public StandardMeshShaderBase {
	inline MaskShader() : ShaderResource() {}
	inline MaskShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "mask.shader", shaderSource) {}

	void updateColor(const Color& color);
};

struct DepthShader : public StandardMeshShaderBase {
	inline DepthShader() : ShaderResource() {}
	inline DepthShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "depth.shader", shaderSource) {}

	void updateLight(const Mat4f& lightMatrix);
};

struct PostProcessShader : public ShaderResource {
	inline PostProcessShader() : ShaderResource() {}
	inline PostProcessShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "postprocess.shader", shaderSource) {}

	void updateTexture(Graphics::Texture* texture);
	void updateTexture(Graphics::HDRTexture* texture);
};

struct OriginShader : public ShaderResource {
	inline OriginShader() : ShaderResource() {}
	inline OriginShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "origin.shader", shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& rotatedViewMatrix, const Mat4f& projectionMatrix, const Mat4f& orthoMatrix, const Position& viewPosition);
};

struct FontShader : public ShaderResource {
	inline FontShader() : ShaderResource() {}
	inline FontShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "font.shader", shaderSource) {}

	void updateColor(const Color& color);
	void updateProjection(const Mat4f& projectionMatrix);
	void updateTexture(Graphics::Texture* texture);
};

struct VectorShader : public ProjectionShaderBase {
	inline VectorShader() : ShaderResource() {}
	inline VectorShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "vector.shader", shaderSource) {}
};

struct PointShader : public ProjectionShaderBase {
	inline PointShader() : ShaderResource() {}
	inline PointShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "point.shader", shaderSource) {}
};

struct TestShader : public StandardMeshShaderBase {
	inline TestShader() : ShaderResource() {}
	inline TestShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "test.shader", shaderSource) {}

	void updateDisplacement(Graphics::Texture* displacementMap);
};

struct LineShader : public ProjectionShaderBase {
	inline LineShader() : ShaderResource() {}
	inline LineShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "line.shader", shaderSource) {}
};

struct SkyShader : public ProjectionShaderBase {
	inline SkyShader() : ShaderResource() {}
	inline SkyShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "sky.shader", shaderSource) {}

	void updateTime(float time);
};

struct LightingShader : public BasicShaderBase {
	inline LightingShader() : ShaderResource() {}
	inline LightingShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, "lighting.shader", shaderSource) {}
};

namespace Shaders {
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
extern LightingShader lightingShader;
extern DebugShader debugShader;

void onInit();
void onClose();
}

};