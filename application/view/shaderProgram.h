#pragma once

#include "shader.h"

class HDRTexture;
class Texture;
class CubeMap;
struct Light;
struct ExtendedPart;
struct Material;

struct ShaderProgram {
public:
	Shader shader;
	ShaderSource shaderSource;

	ShaderProgram() {};
	ShaderProgram(const ShaderProgram&) = delete;

	template <class ...Args>
	ShaderProgram(ShaderSource shaderSource, const Args&... args);

	void bind();
	void close();

	~ShaderProgram();
};

struct SkyboxShader : public ShaderProgram {
	SkyboxShader() : ShaderProgram() {}
	SkyboxShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "viewMatrix", "projectionMatrix", "skyboxTexture", "lightDirection") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix);
	void updateCubeMap(CubeMap* skybox);
	void updateLightDirection(const Vec3f& lightDirection);
};

struct MaskShader : public ShaderProgram {
	MaskShader() : ShaderProgram() {}
	MaskShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "viewMatrix", "projectionMatrix", "modelMatrix", "color") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix);
	void updateModel(const Mat4f& modelMatrix);
	void updateColor(const Vec4f& color);
};

struct BasicShader : public ShaderProgram {
	BasicShader() : ShaderProgram() {}
	BasicShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "modelMatrix", "viewMatrix", "projectionMatrix", "viewPosition", "includeNormals", "includeUvs", "material.ambient", "material.diffuse", "material.specular", "material.reflectance", "material.textured", "material.normalmapped", "textureSampler", "normalSampler", "sunDirection", "sunColor", "gamma", "hdr", "exposure") {}

	void createLightArray(int size);
	void updateSunDirection(const Vec3f& sunDirection);
	void updateSunColor(const Vec3f& sunColor);
	void updateGamma(float gamma);
	void updateHDR(bool hdr);
	void updateExposure(float exposure);
	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
	void updateLight(Light lights[], int size);
	void updatePart(const ExtendedPart& part);
	void updateMaterial(const Material& material);
	void updateModel(const Mat4f& modelMatrix);

};

struct DepthShader : public ShaderProgram {
	DepthShader() : ShaderProgram() {}
	DepthShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "modelMatrix", "lightMatrix") {}

	void updateLight(const Mat4f& lightMatrix);
	void updateModel(const Mat4f& modelMatrix);
};

struct QuadShader : public ShaderProgram {
	QuadShader() : ShaderProgram() {}
	QuadShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "projectionMatrix", "color", "textureSampler", "textured") {}

	void updateProjection(const Mat4f& orthoMatrix);
	void updateColor(const Vec4& color);
	void updateTexture(Texture* texture);
	void updateTexture(Texture* texture, const Vec4f& color);
};

struct BlurShader : public ShaderProgram {
	BlurShader() : ShaderProgram() {}
	BlurShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "image", "horizontal") {}

	enum class BlurType {
		HORIZONTAL = 0,
		VERTICAL = 1
	};

	void updateType(BlurType type);
	void updateTexture(Texture* texture);
};

struct PostProcessShader : public ShaderProgram {
	PostProcessShader() : ShaderProgram() {}
	PostProcessShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "textureSampler") {}

	void updateTexture(Texture* texture);
	void updateTexture(HDRTexture* texture);
};

struct OriginShader : public ShaderProgram {
	OriginShader() : ShaderProgram() {}
	OriginShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "viewMatrix", "rotatedViewMatrix", "projectionMatrix", "orthoMatrix", "viewPosition") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& rotatedViewMatrix, const Mat4f& projectionMatrix, const Mat4f& orthoMatrix, const Position& viewPosition);
};

struct FontShader : public ShaderProgram {
	FontShader() : ShaderProgram() {}
	FontShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "projectionMatrix", "color", "text") {}

	void updateColor(const Vec4f& color);
	void updateProjection(const Mat4f& projectionMatrix);
	void updateTexture(Texture* texture);
};

struct VectorShader : public ShaderProgram {
	VectorShader() : ShaderProgram() {}
	VectorShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "viewMatrix", "projectionMatrix", "viewPosition") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
};

struct PointShader : public ShaderProgram {
	PointShader() : ShaderProgram() {}
	PointShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "viewMatrix", "projectionMatrix", "viewPosition") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition);
};

struct TestShader : public ShaderProgram {
	TestShader() : ShaderProgram() {}
	TestShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "displacementMap", "viewMatrix", "modelMatrix", "projectionMatrix", "viewPosition") {}

	void updateProjection(const Mat4f& projectionMatrix);
	void updateView(const Mat4f& viewMatrix);
	void updateModel(const Mat4f& modelMatrix);
	void updateViewPosition(const Position& viewPosition);
	void updateDisplacement(Texture* displacementMap);
};

struct LineShader : public ShaderProgram {
	LineShader() : ShaderProgram() {}
	LineShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "projectionMatrix", "viewMatrix") {}

	void updateProjection(const Mat4f& projectionMatrix, const Mat4f& viewMatrix);
};

struct EdgeShader : public ShaderProgram {
	EdgeShader() : ShaderProgram() {}
	EdgeShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "textureSampler") {}

	void updateTexture(Texture* texture);
	void updateTexture(HDRTexture* texture);
};

struct GuiShader : public ShaderProgram {
	GuiShader() : ShaderProgram() {}
	GuiShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "projectionMatrix", "textureSampler", "textured") {}

	void init(const Mat4f& orthoMatrix);
	void setTextured(bool textured);
};

namespace Shaders {
	extern BasicShader basicShader;
	extern DepthShader depthShader;
	extern VectorShader vectorShader;
	extern OriginShader originShader;
	extern FontShader fontShader;
	extern QuadShader quadShader;
	extern PostProcessShader postProcessShader;
	extern SkyboxShader skyboxShader;
	extern PointShader pointShader;
	extern TestShader testShader;
	extern BlurShader blurShader;
	extern LineShader lineShader;
	extern EdgeShader edgeShader;
	extern MaskShader maskShader;
	extern GuiShader guiShader;

	void onInit();
	void onClose();
}
