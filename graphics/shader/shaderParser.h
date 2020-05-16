#pragma once

#include <stack>
#include "shaderLexer.h"

namespace Graphics {

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
	std::string variableType;
	bool array;
	int amount;

	ShaderLocal(const std::string& name, const std::string& variableType, bool array, int amount) : name(name), variableType(variableType), array(array), amount(amount) {}
};
typedef std::vector<ShaderLocal> ShaderLocals;

struct ShaderGlobal {
	std::string name;
	std::string ioType;
	std::string variableType;
	bool array;
	int amount;

	ShaderGlobal(const std::string& name, const std::string& ioType, const std::string& variableType, bool array, int amount) : name(name), ioType(ioType), variableType(variableType), array(array), amount(amount) {};
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
	std::string ioType;
	std::string variableType;
	std::string name;

	ShaderLayoutItem(const ShaderLayoutAttributes& attributes, const std::string& ioType, const std::string& variableType, const std::string& name) : attributes(attributes), ioType(ioType), variableType(variableType), name(name) {}
};
typedef std::vector<ShaderLayoutItem> ShaderLayout;

struct ShaderVSOUT : public ShaderGlobal {
	ShaderLocals locals;

	ShaderVSOUT(const std::string& name, const std::string& ioType, const std::string& variableType, bool array, int amount, const ShaderLocals& locals) : ShaderGlobal(name, ioType, variableType, array, amount), locals(locals) {}
};

struct ShaderStruct {
	std::string name;
	ShaderLocals locals;

	ShaderStruct() {};
	ShaderStruct(const std::string& name, const ShaderLocals& locals) : name(name), locals(locals) {}
};
typedef std::unordered_map<std::string, ShaderStruct> ShaderStructs;

typedef ShaderLocal ShaderUniform;
typedef std::vector<ShaderUniform> ShaderUniforms;
typedef std::unordered_map<std::string, float> ShaderDefines;

struct ShaderInfo {
	ShaderLayout layout;
	ShaderUniforms uniforms;
	ShaderGlobals globals;
	ShaderLocals locals;
	ShaderDefines defines;
	ShaderStructs structs;

	ShaderInfo() {}
	ShaderInfo(const ShaderLayout& layout, const ShaderUniforms& uniforms, const ShaderGlobals& globals, const ShaderLocals& locals, const ShaderDefines& defines, const ShaderStructs& structs) : layout(layout), uniforms(uniforms), globals(globals), locals(locals), defines(defines), structs(structs) {}
};

class ShaderParser {
public:
	static ShaderVariableType parseVariableType(const std::string& value);
	static ShaderInfo parse(const std::string& code);
	static ShaderInfo parseTokens(TokenStack& tokens);
};

};