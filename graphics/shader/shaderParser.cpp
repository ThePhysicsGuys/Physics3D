#include "core.h"

#include "shaderParser.h"

ShaderIOType parseIOType(const Token& token) {
	std::string value = token.value;
	if (value == "in")
		return ShaderIOType::IN;
	if (value == "out")
		return ShaderIOType::OUT;

	return ShaderIOType::NONE;
}

ShaderVariableType parseVariableType(const Token& token) {
	std::string value = token.value;
	if (value == "void")
		return ShaderVariableType::VOID;
	if (value == "int")
		return ShaderVariableType::INT;
	if (value == "float")
		return ShaderVariableType::FLOAT;
	if (value == "mat2")
		return ShaderVariableType::MAT2;
	if (value == "mat3")
		return ShaderVariableType::MAT3;
	if (value == "mat4")
		return ShaderVariableType::MAT4;
	if (value == "vec2")
		return ShaderVariableType::VEC2;
	if (value == "vec3")
		return ShaderVariableType::VEC3;
	if (value == "vec4")
		return ShaderVariableType::VEC4;
	if (value == "sampler2D")
		return ShaderVariableType::SAMPLER2D;
	if (value == "sampler3D")
		return ShaderVariableType::SAMPLER3D;
	if (value == "struct")
		return ShaderVariableType::STRUCT;

	return ShaderVariableType::NONE;
}

ShaderUniform parseUniform(std::vector<Token>::iterator& iterator) {
	ShaderVariableType variableType = parseVariableType(*++iterator);
	std::string name = (++iterator)->value;
	
	return { name, variableType };
}

ShaderVSOUT parseVSOUT(std::vector<Token>::iterator& iterator, const ShaderIOType& type) {

}

ShaderGlobal parseGlobal(std::vector<Token>::iterator& iterator) {
	ShaderIOType ioType = parseIOType(*iterator);
	ShaderVariableType variableType = parseVariableType(*++iterator);

	switch (variableType) {
		case ShaderVariableType::STRUCT: {
			ShaderVSOUT vsout = parseVSOUT(iterator, ioType);
			return vsout;
			break;
		} default: {
			std::string name = (++iterator)->value;

			bool array = false;
			if ((++iterator)->type == TokenType::LB)
				if ((++iterator)->type == TokenType::RB)
					array = true;

			return { name, array, ioType, variableType };
			break; }
	}
}

ShaderLocal parseLocal(std::vector<Token>::iterator& iterator) {

}

ShaderLayoutItem parseLayoutItem(std::vector<Token>::iterator& iterator) {

}

ShaderInfo Parser::parse(std::vector<Token> tokens) {
	auto iterator = tokens.begin();

	ShaderLayout layout;
	ShaderUniforms uniforms;
	ShaderGlobals globals;
	ShaderLocals locals;

	while (iterator != tokens.end()) {
		const Token& token = *iterator;
		switch (token.type) {
			case TokenType::UNIFORM: {
				ShaderUniform uniform = parseUniform(iterator);
				uniforms.push_back(uniform);
				break; 
			} case TokenType::IO: {
				ShaderGlobal global = parseGlobal(iterator);
				globals.push_back(global);
				break;
			} case TokenType::LAYOUT: {
				ShaderLayoutItem layoutItem = parseLayoutItem(iterator);
				layout.push_back(layoutItem);
				break;
			} case TokenType::TYPE: {
				ShaderLocal local = parseLocal(iterator);
				locals.push_back(local);
				break;
			}
		}
	}
}
