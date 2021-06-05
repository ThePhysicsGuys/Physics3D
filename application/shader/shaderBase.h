#pragma once

#include <Physics3D/math/position.h>
#include <Physics3D/math/globalCFrame.h>
#include <Physics3D/math/linalg/mat.h>

#include "../graphics/resource/shaderResource.h"

namespace P3D::Application {
	namespace Comp {
		struct Light;
	}

	using namespace Graphics;

struct Light;

struct ProjectionShaderBase : public virtual ShaderResource {
	inline ProjectionShaderBase() : ShaderResource() {}
	inline ProjectionShaderBase(const std::string& name, const std::string& path, const ShaderSource& source) : ShaderResource(name, path, source) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
	void updateProjectionMatrix(const Mat4f& projectionMatrix);
	void updateViewMatrix(const Mat4f& viewMatrix);
	void updateViewPosition(const Position& viewPosition);
};

struct StandardMeshShaderBase : public ProjectionShaderBase {
	inline StandardMeshShaderBase() : ProjectionShaderBase() {}
	inline StandardMeshShaderBase(const std::string& name, const std::string& path, const ShaderSource& source) : ProjectionShaderBase(name, path, source) {}

	void updateModel(const Mat4f& modelMatrix);
	void updateModel(const GlobalCFrame& modelCFrame, const DiagonalMat3f& scale);
};

struct InstancedMeshShaderBase : public ProjectionShaderBase {
	inline InstancedMeshShaderBase() : ProjectionShaderBase() {}
	inline InstancedMeshShaderBase(const std::string& name, const std::string& path, const ShaderSource& source) : ProjectionShaderBase(name, path, source) {}
};

struct BasicShaderBase : public virtual ShaderResource {
	inline BasicShaderBase() : ShaderResource() {}
	inline BasicShaderBase(const std::string& name, const std::string& path, const ShaderSource& source) : ShaderResource(name, path, source) {}

	void updateSunDirection(const Vec3f& sunDirection);
	void updateSunColor(const Vec3f& sunColor);
	void updateGamma(float gamma);
	void updateHDR(float hdr);
	void updateExposure(float exposure);
	void updateLightCount(std::size_t lightCount);
	void updateLight(std::size_t index, const Position& position, const Comp::Light& light);
};

}