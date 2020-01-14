#pragma once

#include "../bindable.h"
#include "shaderParser.h"

struct ShaderSource {
	std::string name;

	std::string vertexSource;
	std::string fragmentSource;
	std::string geometrySource;
	std::string tesselationControlSource;
	std::string tesselationEvaluateSource;

	ShaderSource();
	ShaderSource(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource, const std::string& tesselationControlSource, const std::string& tesselationEvaluateSource);
};

struct ShaderStage {
	std::string source;
	ShaderInfo info;

	ShaderStage();
	ShaderStage(const std::string& source, const ShaderInfo& info);
};

class Shader : public Bindable {
private:
	std::map<std::string, int> uniforms;

public:
	enum ShaderFlags : char {
		NONE                 = 0 << 0,
		VERTEX               = 1 << 0,
		FRAGMENT             = 1 << 1,
		GEOMETRY             = 1 << 2,
		TESSELATION_CONTROL  = 1 << 2,
		TESSELATION_EVALUATE = 1 << 3
	};

	char flags = NONE;

	std::string name;

	ShaderStage vertexStage;
	ShaderStage fragmentStage;
	ShaderStage geometryStage;
	ShaderStage tesselationControlStage;
	ShaderStage tesselationEvaluationStage;

	Shader();
	Shader(const ShaderSource& shaderSource);
	Shader(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader);
	Shader(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader);
	Shader(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string& tesselationControlSource, const std::string& tesselationEvaluateSource);

	~Shader();
	Shader(Shader&& other);
	Shader(const Shader&) = delete;
	Shader& operator=(Shader&& other);
	Shader& operator=(const Shader&) = delete;

	void createUniform(const std::string& uniform);
	void setUniform(const std::string& uniform, int value) const;
	void setUniform(const std::string& uniform, float value) const;
	void setUniform(const std::string& uniform, const Vec2f& value) const;
	void setUniform(const std::string& unfiorm, const Vec3f& value) const;
	void setUniform(const std::string& unfiorm, const Vec4f& value) const;
	void setUniform(const std::string& uniform, const Mat4f& value) const;
	void setUniform(const std::string& uniform, const Position& value) const;

	void bind() override;
	void unbind() override;
	void close() override;
};

ShaderSource parseShader(const std::string& name, std::istream& shaderTextStream);
ShaderSource parseShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
ShaderSource parseShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);
ShaderSource parseShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, const std::string& tesselationControlPath, const std::string& tesselationEvaluatePath);