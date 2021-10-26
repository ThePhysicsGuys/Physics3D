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

struct ProjectionShaderBase : public virtual ShaderResource {
	ProjectionShaderBase(const std::string& name, const std::string& path, bool isPath = true) : ShaderResource(name, path, isPath) {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
	void updateProjectionMatrix(const Mat4f& projectionMatrix);
	void updateViewMatrix(const Mat4f& viewMatrix);
	void updateViewPosition(const Position& viewPosition);
};

struct StandardMeshShaderBase : public ProjectionShaderBase {
	StandardMeshShaderBase(const std::string& name, const std::string& path, bool isPath = true) : ProjectionShaderBase(name, path, isPath) {}

	void updateModel(const Mat4f& modelMatrix);
	void updateModel(const GlobalCFrame& modelCFrame, const DiagonalMat3f& scale);
};

struct InstancedMeshShaderBase : public ProjectionShaderBase {
	InstancedMeshShaderBase(const std::string& name, const std::string& path, bool isPath = true) : ProjectionShaderBase(name, path, isPath) {}
};

struct BasicShaderBase : public virtual ShaderResource {
	BasicShaderBase(const std::string& name, const std::string& path, bool isPath = true) : ShaderResource(name, path, isPath) {}

	void updateSunDirection(const Vec3f& sunDirection);
	void updateSunColor(const Vec3f& sunColor);
	void updateGamma(float gamma);
	void updateHDR(float hdr);
	void updateExposure(float exposure);
	void updateLightCount(std::size_t lightCount);
	void updateLight(std::size_t index, const Position& position, const Comp::Light& light);
};

}