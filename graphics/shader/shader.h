#pragma once

#include "../bindable.h"

struct ShaderSource {
	std::string vertexSource;
	std::string fragmentSource;
	std::string geometrySource;
	std::string tesselationControlSource;
	std::string tesselationEvaluateSource;
	std::string name;
};

ShaderSource parseShader(const std::string& name, std::istream& shaderTextStream);
ShaderSource parseShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
ShaderSource parseShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath);
ShaderSource parseShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, const std::string& tesselationControlPath, const std::string& tesselationEvaluatePath);

class Shader : public Bindable {
	enum ShaderFlags : char {
		NONE                 = 0 << 0,
		VERTEX               = 1 << 0,
		FRAGMENT             = 1 << 1,
		GEOMETRY             = 1 << 2,
		TESSELATION_CONTROL  = 1 << 2,
		TESSELATION_EVALUATE = 1 << 3
	};

private:
	char flags = NONE;
	std::map<std::string, int> uniforms;
	std::vector<std::string> vertexUniforms;
	std::vector<std::string> fragmentUniforms;
	std::vector<std::string> geometryUniforms;
	std::vector<std::string> tesselationControlUniforms;
	std::vector<std::string> tesselationEvaluateUniforms;

public:
	std::string name;
	std::string vertexSource;
	std::string fragmentSource;
	std::string geometrySource;
	std::string tesselationControlSource;
	std::string tesselationEvaluateSource;

	Shader() {};
	Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& name) : Shader(vertexShader, fragmentShader, "", "", "", name) {};
	Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string& name) : Shader(vertexShader, fragmentShader, geometryShader, "", "", name) {};
	Shader(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string& tesselationControlSource, const std::string& tesselationEvaluateSource);
	Shader(const ShaderSource& shaderSource) : Shader(shaderSource.vertexSource, shaderSource.fragmentSource, shaderSource.geometrySource, shaderSource.tesselationControlSource, shaderSource.tesselationEvaluateSource, shaderSource.name) {};

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
