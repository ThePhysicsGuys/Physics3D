#include "core.h"

#include "parser.h"
#include "lexer.h"

#include <sstream>

namespace P3D::Graphics {

	Parser::Type Parser::parseType(std::string_view type) {
		if (type == "void")
			return Type::Void;
		if (type == "bool")
			return Type::Bool;
		if (type == "int")
			return Type::Int;
		if (type == "float")
			return Type::Float;
		if (type == "mat2")
			return Type::Mat2;
		if (type == "mat3")
			return Type::Mat3;
		if (type == "mat4")
			return Type::Mat4;
		if (type == "vec2")
			return Type::Vec2;
		if (type == "vec3")
			return Type::Vec3;
		if (type == "vec4")
			return Type::Vec4;
		if (type == "struct")
			return Type::Struct;
		if (type == "VS_OUT")
			return Type::VSOut;
		if (type == "struct")
			return Type::Struct;
		if (type == "sampler2d")
			return Type::Sampler2D;
		if (type == "sampler3d")
			return Type::Sampler3D;

		return Type::None;
	}

	Parser::IO Parser::parseIO(std::string_view io) {
		if (io == "in")
			return IO::In;
		if (io == "out")
			return IO::Out;

		return IO::None;
	}

	void parseUntil(Lexer& lexer, Lexer::Token::Type ltype, Lexer::Token::Type rtype) {
		int depth = -1;
		Lexer::Token next = lexer.next();
		while (next.type != Lexer::Token::End) {
			if (next.type == ltype) {
				depth++;
			} else if (next.type == rtype) {
				if (depth == 0)
					return;

				if (depth > 0)
					depth--;
			}
			
			next = lexer.next();
		}
	}

	void parseUntil(Lexer& lexer, Lexer::Token::Type type) {
		Lexer::Token next = lexer.next();

		while (next.type != Lexer::Token::End) {
			if (next.type == type)
				return;
			
			next = lexer.next();
		}
	}

	std::size_t parseArray(Lexer& lexer, Parser::Parse& result) {
		Lexer::Token leftBracket = lexer.peek();
		if (leftBracket.type == Lexer::Token::LeftBracket) {
			lexer.advance(leftBracket.size());

			Lexer::Token next = lexer.next();
			switch (next.type) {
				case Lexer::Token::Number: {
					std::size_t amount;
					std::stringstream stream(std::string(next.first(lexer.code), next.size()));
					stream >> amount;

					if (stream.fail())
						return static_cast<std::size_t>(-1);
					
					return amount;
				} case Lexer::Token::Identifier: {
					std::string define(next.string(lexer.code));
					if (auto iterator = result.defines.find(define); iterator != result.defines.end())
						return static_cast<int>(iterator->second);
					return static_cast<std::size_t>(-1);
				} case Lexer::Token::RightBracket: {
					return 0;
				} default:
					return static_cast<std::size_t>(-1);
			}
		}

		return 0;
	}

	void parseUniform(Lexer& lexer, const Lexer::Token& current, Parser::Parse& result) {
		Lexer::Token type = lexer.next();
		if (type.type != Lexer::Token::Datatype && type.type != Lexer::Token::Identifier)
			return;

		if (type.type == Lexer::Token::Identifier && result.structs.find(type.string(lexer.code)) == result.structs.end())
			return;
		
		Lexer::Token name = lexer.next();
		if (name.type != Lexer::Token::Identifier)
			return;

		std::size_t amount = parseArray(lexer, result);
		if (amount == static_cast<std::size_t>(-1))
			return;

		parseUntil(lexer, Lexer::Token::Semicolon);

		result.uniforms.push_back(Parser::Local { type.range, name.range, amount });
	}

	Parser::Locals parseScope(Lexer& lexer, Parser::Parse& result) {
		Parser::Locals locals;
		
		Lexer::Token next = lexer.next();
		if (next.type != Lexer::Token::LeftCurl)
			return locals;

		int depth = 1;
		while (next.type != Lexer::Token::End) {
			next = lexer.next();

			if (next.type == Lexer::Token::LeftCurl) {
				depth++;
				continue;
			}

			if (next.type == Lexer::Token::RightCurl) {
				depth--;
				
				if (depth == 0)
					break;
				
				continue;
			}

			if (next.type == Lexer::Token::Datatype || next.type == Lexer::Token::Identifier && result.structs.find(next.string(lexer.code)) != result.structs.end()) {
				Lexer::Token type = next;
				
				Lexer::Token name = lexer.next();
				if (name.type != Lexer::Token::Identifier)
					continue;

				std::size_t amount = parseArray(lexer, result);
				if (amount == static_cast<std::size_t>(-1))
					continue;

				locals.push_back(Parser::Local { type.range, name.range, amount });
			}
		}

		return locals;
	}

	void parseVSOut(Lexer& lexer, const Lexer::Token& type, const Parser::IO& io, Parser::Parse& result) {
		Parser::Locals locals = parseScope(lexer, result);

		Lexer::Token name = lexer.next();
		if (name.type != Lexer::Token::Identifier)
			return;

		std::size_t amount = parseArray(lexer, result);
		if (amount == static_cast<std::size_t>(-1))
			return;

		result.vsOuts.push_back(Parser::VSOut { io, type.range, name.range, amount, locals });
	}

	void parseGlobal(Lexer& lexer, const Lexer::Token& current, Parser::Parse& result) {
		Parser::IO io = Parser::parseIO(current.view(lexer.code));
		if (io == Parser::IO::None)
			return;

		Lexer::Token type = lexer.next();
		if (type.type != Lexer::Token::Datatype)
			return;

		if (Parser::parseType(type.view(lexer.code)) == Parser::Type::VSOut) {
			parseVSOut(lexer, type, io, result);
			return;
		}

		Lexer::Token name = lexer.next();
		if (name.type != Lexer::Token::Identifier)
			return;
		
		std::size_t amount = parseArray(lexer, result);
		if (amount == static_cast<std::size_t>(-1))
			return;

		result.globals.emplace_back(Parser::Global { io, type.range, name.range, amount });
	}

	void parseVersion(Lexer& lexer, const Lexer::Token& current, Parser::Parse& result) {
		Lexer::Token versionToken = lexer.next();
		if (versionToken.type != Lexer::Token::Number)
			return;

		std::size_t version;
		std::stringstream stream(std::string(versionToken.first(lexer.code), versionToken.size()));
		stream >> version;

		if (stream.fail())
			return;
		
		bool core = false;
		Lexer::Token coreToken = lexer.peek();
		if (coreToken.type == Lexer::Token::Identifier && coreToken.view(lexer.code) == "core") {
			lexer.advance(coreToken.size());
			core = true;
		}

		result.version = Parser::Version { version, core };
	}

	void parseDefine(Lexer& lexer, Parser::Parse& result) {
		Lexer::Token name = lexer.next();
		if (name.type != Lexer::Token::Identifier)
			return;

		int number = 1;
		Lexer::Token numberToken = lexer.next();
		if (numberToken.type == Lexer::Token::Number) {
			std::stringstream stream(std::string(numberToken.first(lexer.code), numberToken.size()));
			stream >> number;

			if (stream.fail())
				return;
		}

		result.defines[name.string(lexer.code)] = number;
	}
	
	void parsePreprocessor(Lexer& lexer, const Lexer::Token& current, Parser::Parse& result) {
		if (current.view(lexer.code) == "#define")
			parseDefine(lexer, result);
	}

	void parseIdentifier(Lexer& lexer, const Lexer::Token& current, Parser::Parse& result) {
		Lexer::Token type = current;
		if (result.structs.find(current.string(lexer.code)) == result.structs.end())
			return;
		
		Lexer::Token name = lexer.next();
		if (name.type != Lexer::Token::Identifier)
			return;

		std::size_t amount = parseArray(lexer, result);
		if (amount == static_cast<std::size_t>(-1))
			return;

		result.locals.push_back(Parser::Local { type.range, name.range, amount });
	}

	void parseDatatype(Lexer& lexer, const Lexer::Token& current, Parser::Parse& result) {
		if (Parser::parseType(current.view(lexer.code)) == Parser::Type::Void || lexer.peek().type == Lexer::Token::Identifier && lexer.peek(1).type == Lexer::Token::LeftParenthesis) 
			parseUntil(lexer, Lexer::Token::LeftCurl, Lexer::Token::RightCurl);
		
	}
	
	void parseStruct(Lexer& lexer, const Lexer::Token& current, Parser::Parse& result) {
		Lexer::Token name = lexer.next();
		if (name.type != Lexer::Token::Identifier)
			return;

		Parser::Locals locals = parseScope(lexer, result);

		result.structs[name.string(lexer.code)] = Parser::Struct { name.range, locals };
	}
	
	Parser::Parse Parser::parse(const char* code) {
		Parse result;

		Lexer lexer(code);
		Lexer::Token current = lexer.next();
		while (current.type != Lexer::Token::End) {
			switch (current.type) {
				case Lexer::Token::Uniform: {
					parseUniform(lexer, current, result);
					break;
				} case Lexer::Token::InOut: {
					parseGlobal(lexer, current, result);
					break;
				} case Lexer::Token::Datatype: {
					parseDatatype(lexer, current, result);
					break;
				} case Lexer::Token::Layout: {
					// TODO
					break;	
				} case Lexer::Token::Version: {
					parseVersion(lexer, current, result);
					break;
				} case Lexer::Token::Preprocessor: {
					parsePreprocessor(lexer, current, result);
					break;
				} case Lexer::Token::Struct: {
					parseStruct(lexer, current, result);
					break;
				} case Lexer::Token::Identifier: {
					parseIdentifier(lexer, current, result);
					break;
				} default:
					break;
			}
			
			current = lexer.next();
		}

		return result;
	}

	
}
