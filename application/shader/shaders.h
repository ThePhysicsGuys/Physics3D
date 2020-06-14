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

struct DepthBufferShader : public ShaderResource {
	inline DepthBufferShader() : ShaderResource() {}
	inline DepthBufferShader(const ShaderSource& shaderSource) : ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}

	void updateDepthMap(GLID unit, GLID id);
	void updatePlanes(float near, float far);
};

struct DebugShader : public StandardMeshShaderBase {
	inline DebugShader() : StandardMeshShaderBase(), ShaderResource() {}
	inline DebugShader(const ShaderSource& shaderSource) : StandardMeshShaderBase(shaderSource.name, shaderSource.path, shaderSource), ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}
};

struct SkyboxShader : public ProjectionShaderBase {
	inline SkyboxShader() : ProjectionShaderBase(), ShaderResource() {}
	inline SkyboxShader(const ShaderSource& shaderSource) : ProjectionShaderBase(shaderSource.name, shaderSource.path, shaderSource), ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}

	void updateCubeMap(Graphics::CubeMap* skybox);
	void updateLightDirection(const Vec3f& lightDirection);
};

struct MaskShader : public StandardMeshShaderBase {
	inline MaskShader() : StandardMeshShaderBase(), ShaderResource() {}
	inline MaskShader(const ShaderSource& shaderSource) : StandardMeshShaderBase(shaderSource.name, shaderSource.path, shaderSource), ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}

	void updateColor(const Color& color);
};

struct DepthShader : public StandardMeshShaderBase {
	inline DepthShader() : StandardMeshShaderBase(), ShaderResource() {}
	inline DepthShader(const ShaderSource& shaderSource) : StandardMeshShaderBase(shaderSource.name, shaderSource.path, shaderSource), ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}

	void updateLight(const Mat4f& lightMatrix);
};

struct PostProcessShader : public ShaderResource {
	inline PostProcessShader() : ShaderResource() {}
	inline PostProcessShader(const ShaderSource& shaderSource) : ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}

	void updateTexture(Graphics::Texture* texture);
	void updateTexture(Graphics::HDRTexture* texture);
};

struct OriginShader : public ShaderResource {
	inline OriginShader() : ShaderResource() {}
	inline OriginShader(const ShaderSource& shaderSource) : ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& rotatedViewMatrix, const Mat4f& projectionMatrix, const Mat4f& orthoMatrix, const Position& viewPosition);
};

struct FontShader : public ShaderResource {
	inline FontShader() : ShaderResource() {}
	inline FontShader(const ShaderSource& shaderSource) : ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}

	void updateColor(const Color& color);
	void updateProjection(const Mat4f& projectionMatrix);
	void updateTexture(Graphics::Texture* texture);
};

struct VectorShader : public ProjectionShaderBase {
	inline VectorShader() : ProjectionShaderBase(), ShaderResource() {}
	inline VectorShader(const ShaderSource& shaderSource) : ProjectionShaderBase(shaderSource.name, shaderSource.path, shaderSource), ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}
};

struct PointShader : public ProjectionShaderBase {
	inline PointShader() : ProjectionShaderBase(), ShaderResource() {}
	inline PointShader(const ShaderSource& shaderSource) : ProjectionShaderBase(shaderSource.name, shaderSource.path, shaderSource), ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}
};

struct TestShader : public StandardMeshShaderBase {
	inline TestShader() : StandardMeshShaderBase(), ShaderResource() {}
	inline TestShader(const ShaderSource& shaderSource) : StandardMeshShaderBase(shaderSource.name, shaderSource.path, shaderSource), ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}

	void updateDisplacement(Graphics::Texture* displacementMap);
};

struct LineShader : public ProjectionShaderBase {
	inline LineShader() : ProjectionShaderBase(), ShaderResource() {}
	inline LineShader(const ShaderSource& shaderSource) : ProjectionShaderBase(shaderSource.name, shaderSource.path, shaderSource), ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}
};

struct SkyShader : public ProjectionShaderBase {
	inline SkyShader() : ProjectionShaderBase(), ShaderResource() {}
	inline SkyShader(const ShaderSource& shaderSource) : ProjectionShaderBase(shaderSource.name, shaderSource.path, shaderSource), ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}

	void updateTime(float time);
};

struct LightingShader : public BasicShaderBase {
	inline LightingShader() : BasicShaderBase(), ShaderResource() {}
	inline LightingShader(const ShaderSource& shaderSource) : BasicShaderBase(shaderSource.name, shaderSource.path, shaderSource), ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}
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
extern DepthBufferShader depthBufferShader;

void onInit();
void onClose();
}

};