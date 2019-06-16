#pragma once

#include <iostream>
#include <string>
#include <map>

#include "bindable.h"

#include "../engine/math/vec2.h"
#include "../engine/math/mat3.h"
#include "../engine/math/mat4.h"

struct ShaderSource {
	std::string vertexSource;
	std::string fragmentSource;
	std::string geometrySource;
	std::string tesselationControlSource;
	std::string tesselationEvaluateSource;
	std::string name;
};

ShaderSource parseShader(std::istream& shaderTextStream, const std::string name);
ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string name);
ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, const std::string name);
ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, const std::string& tesselationControlPath, const std::string& tesselationEvaluatePath, const std::string name);

class Shader : public Bindable {
private:
	std::map<std::string, int> uniforms;
public:
	std::string name;
	Shader() {};
	Shader(const Shader&) = delete;
	Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string name);
	Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string name);
	Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string& tesselationControlSource, const std::string& tesselationEvaluateSource, const std::string name);
	Shader(ShaderSource shaderSource) : Shader(shaderSource.vertexSource, shaderSource.fragmentSource, shaderSource.geometrySource, shaderSource.tesselationControlSource, shaderSource.tesselationEvaluateSource, shaderSource.name) {};

	void createUniform(std::string uniform);
	void setUniform(std::string uniform, int value) const;
	void setUniform(std::string uniform, float value) const;
	void setUniform(std::string uniform, double value) const;
	void setUniform(std::string uniform, Vec2f value) const;
	void setUniform(std::string unfiorm, Vec3f value) const;
	void setUniform(std::string unfiorm, Vec4f value) const;
	void setUniform(std::string uniform, Mat4f value) const;

	void bind() override;
	void unbind() override;
	void close() override;
};
