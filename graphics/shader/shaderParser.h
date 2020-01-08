#pragma once

#include "shaderLexer.h"

enum ShaderVariableType {
	VOID,
	INT,
	FLOAT,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4,
	STRUCT,
	SAMPLER2D,
	SAMPLER3D
};

enum ShaderIOType {
	IN,
	OUT
};

struct ShaderLayoutItem {
	int location;
	ShaderIOType ioType;
	ShaderVariableType variableType;
};

struct ShaderUniform {
	std::string name;
	ShaderVariableType variableType;
};

struct ShaderGlobal {
	std::string name;
	ShaderIOType ioType;
	ShaderVariableType variableType;
};

typedef std::vector<ShaderLayoutItem> ShaderLayout;
typedef std::vector<ShaderUniform> ShaderUniforms;
typedef std::vector<ShaderGlobal> ShaderGlobals;

struct ShaderInfo {
	ShaderLayout layout;
	ShaderUniforms uniforms;
	ShaderGlobals globals;
};

class Parser {
public:
	static ShaderInfo parse(std::vector<Token> tokens) {
		
	}
};