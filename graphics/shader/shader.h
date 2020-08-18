#pragma once

#include "../bindable.h"
#include "shaderParser.h"

namespace P3D::Graphics {

struct ShaderStage {
	std::string source;
	ShaderInfo info;

	ShaderStage();
	ShaderStage(const std::string& source, const ShaderInfo& info);
};

struct ShaderSource {
	std::string name;
	std::string path;

	std::string vertexSource;
	std::string fragmentSource;
	std::string geometrySource;
	std::string tesselationControlSource;
	std::string tesselationEvaluateSource;
	std::string computeSource;

	ShaderSource();
	ShaderSource(const std::string& name, const std::string& path, const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource, const std::string& tesselationControlSource, const std::string& tesselationEvaluateSource, const std::string& computeSource);
};

class Shader : public Bindable {
public:
	enum ShaderFlag : char {
		NONE = 0 << 0,
		VERTEX = 1 << 0,
		FRAGMENT = 1 << 1,
		GEOMETRY = 1 << 2,
		TESSELATION_CONTROL = 1 << 3,
		TESSELATION_EVALUATE = 1 << 4,
		COMPUTE = 1 << 5
	};

	char flags = NONE;
	std::string name;

	~Shader();
	Shader(const Shader&) = delete;
	Shader& operator=(Shader&& other);
	Shader& operator=(const Shader&) = delete;

	static GLID compile(const std::string& name, const std::string& source, unsigned int type);

	int getUniform(const std::string& uniform) const;
	void createUniform(const std::string& uniform);
	void setUniform(const std::string& uniform, int value) const;
	void setUniform(const std::string& uniform, GLID value) const;
	void setUniform(const std::string& uniform, float value) const;
	void setUniform(const std::string& uniform, const Vec2f& value) const;
	void setUniform(const std::string& unfiorm, const Vec3f& value) const;
	void setUniform(const std::string& unfiorm, const Vec4f& value) const;
	void setUniform(const std::string& uniform, const Mat2f& value) const;
	void setUniform(const std::string& uniform, const Mat3f& value) const;
	void setUniform(const std::string& uniform, const Mat4f& value) const;
	void setUniform(const std::string& uniform, const Position& value) const;

	void bind() override;
	void unbind() override;
	void close() override;

protected:
	std::unordered_map<std::string, int> uniforms;

	Shader();
	Shader(const ShaderSource& shaderSource);
	Shader(Shader&& other);

	void addUniform(const std::string& uniform);
	void addUniforms(const ShaderStage& stage);

	virtual void addShaderStage(const ShaderStage& stage, const ShaderFlag& flag) = 0;
};

ShaderSource parseShader(const std::string& name, const std::string& path);
ShaderSource parseShader(const std::string& name, const std::string& path, std::istream& shaderTextStream);
ShaderSource parseShader(const std::string& name, const std::string& path, const std::string& shaderText);

ShaderStage parseShaderStage(const std::string& source);

};