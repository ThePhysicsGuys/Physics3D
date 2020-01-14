#pragma once

#include <stack>
#include "shaderLexer.h"

enum class ShaderVariableType {
	NONE,
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
	VS_OUT,
	SAMPLER2D,
	SAMPLER3D
};

enum class ShaderIOType {
	NONE,
	IN,
	OUT
};

struct ShaderLocal {
	std::string name;
	ShaderVariableType variableType;
	bool array;

	ShaderLocal(const std::string& name, const ShaderVariableType& variableType, bool array) : name(name), variableType(variableType), array(array) {}
};
typedef std::vector<ShaderLocal> ShaderLocals;

struct ShaderGlobal {
	std::string name;
	ShaderIOType ioType;
	ShaderVariableType variableType;
	bool array;

	ShaderGlobal(const std::string& name, const ShaderIOType& ioType, const ShaderVariableType& variableType, bool array) : name(name), ioType(ioType), variableType(variableType), array(array) {};
};
typedef std::vector<ShaderGlobal> ShaderGlobals;

struct ShaderLayoutAttribute {
	std::string attribute;
	std::string value;

	ShaderLayoutAttribute(const std::string& attribute, const std::string& value) : attribute(attribute), value(value) {}
};
typedef std::vector<ShaderLayoutAttribute> ShaderLayoutAttributes;

struct ShaderLayoutItem {
	ShaderLayoutAttributes attributes;
	ShaderIOType ioType;

	ShaderVariableType variableType;
	std::string name;

	ShaderLayoutItem(const ShaderLayoutAttributes& attributes, const ShaderIOType& ioType, const ShaderVariableType& variableType, const std::string& name) : attributes(attributes), ioType(ioType), variableType(variableType), name(name) {}
};
typedef std::vector<ShaderLayoutItem> ShaderLayout;

struct ShaderVSOUT : public ShaderGlobal {
	ShaderLocals locals;

	ShaderVSOUT(const std::string& name, const ShaderIOType& ioType, const ShaderVariableType& variableType, bool array, const ShaderLocals& locals) : ShaderGlobal(name, ioType, variableType, array), locals(locals) {}
};

struct ShaderStruct : public ShaderLocal {
	ShaderLocals locals;

	ShaderStruct(const std::string& name, const ShaderVariableType& variableType, bool array, const ShaderLocals& locals) : ShaderLocal(name, variableType, array), locals(locals) {}
};

typedef ShaderLocal ShaderUniform;
typedef std::vector<ShaderUniform> ShaderUniforms;

struct ShaderInfo {
	ShaderLayout layout;
	ShaderUniforms uniforms;
	ShaderGlobals globals;
	ShaderLocals locals;

	ShaderInfo() {}
	ShaderInfo(const ShaderLayout& layout, const ShaderUniforms& uniforms, const ShaderGlobals& globals, const ShaderLocals& locals) : layout(layout), uniforms(uniforms), globals(globals), locals(locals) {}
};

class ShaderParser {
public:
	static ShaderInfo parse(TokenStack& tokens);
};