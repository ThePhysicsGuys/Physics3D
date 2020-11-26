#pragma once

#include "shader.h"

namespace P3D::Graphics {

class GShader : public Shader {
private:
	GLID createShader(const ShaderSource& shaderSource);

protected:
	virtual bool addShaderStage(const ShaderStage& stage, const ShaderFlag& flag) override;

public:
	ShaderStage vertexStage;
	ShaderStage fragmentStage;
	ShaderStage geometryStage;
	ShaderStage tesselationControlStage;
	ShaderStage tesselationEvaluationStage;

	GShader();
	GShader(const ShaderSource& shaderSource);
	GShader(const std::string& name, const std::string& path, const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string& tesselationControlSource, const std::string& tesselationEvaluateSource);
	
	~GShader();
	GShader(GShader&& other);
	GShader(const GShader&) = delete;
	GShader& operator=(GShader&& other);
	GShader& operator=(const GShader&) = delete;

	void reload(const ShaderSource& shaderSource);

	void close() override;
};

ShaderSource parseShader(const std::string& name, const std::string& path, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "", const std::string& tesselationControlPath = "", const std::string& tesselationEvaluatePath = "");

};