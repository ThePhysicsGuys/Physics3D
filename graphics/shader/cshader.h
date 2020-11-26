#pragma once

#include "shader.h"

namespace P3D::Graphics {

class CShader : public Shader {
private:
	GLID createShader(const ShaderSource& shaderSource);

protected:
	bool addShaderStage(const ShaderStage& stage, const ShaderFlag& flag) override;

public:
	ShaderStage computeStage;

	CShader();
	CShader(const ShaderSource& shaderSource);
	CShader(const std::string& name, const std::string& path, const std::string& computeShader);

	~CShader();
	CShader(CShader&& other);
	CShader(const CShader&) = delete;
	CShader& operator=(CShader&& other);
	CShader& operator=(const CShader&) = delete;

	void close() override;

	void dispatch(unsigned int x, unsigned int y, unsigned int z);
	void barrier(unsigned int flags);
};

ShaderSource parseShader(const std::string& name, const std::string& path, const std::string& computePath);

};