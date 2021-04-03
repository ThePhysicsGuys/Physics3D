#include "testsMain.h"

#include "../graphics/shader/lexer.h"
#include "../graphics/shader/parser.h"
#include "../util/log.h"

static const char* code = R"(
struct Light {
	vec3 position;
	vec3 color;
	float intensity;
	Attenuation attenuation;
};

#define maxLights 10
uniform Light lights[maxLights];
	)";

static const char* names[] = {
		"Error",
		"Comma",
		"Semicolon",
		"LeftParenthesis",
		"RightParenthesis",
		"Equals",
		"Dot",
		"InOut",
		"Layout",
		"Uniform",
		"Qualifier",
		"Struct",
		"Version",
		"Preprocessor",
		"Datatype",
		"Identifier",
		"LeftCurl",
		"RightCurl",
		"LeftBracket",
		"RightBracket",
		"Number",
		"Operator",
		"SingleQuote",
		"DoubleQuote",
		"String",
		"Comment",
		"End"
};

TEST_CASE(lexVersionPreprocessor) {
	using namespace P3D::Graphics;

	const char* code = R"(
		#version 123;
		#version #ifdef 2 #else 4
	)";

	Lexer lexer(code);

	auto current = lexer.next();
	while (current.type != Lexer::Token::End) {
		Log::debug("%s[%s]", names[current.type], current.string(code).c_str());
		current = lexer.next();
	}
}

TEST_CASE(lexShader) {
	using namespace P3D::Graphics;
	
	Lexer lexer(code);

	auto current = lexer.next();
	while (current.type != Lexer::Token::End) {
		Log::debug("%s[%s]", names[current.type], current.string(code).c_str());
		current = lexer.next();
	}
}

TEST_CASE(parseShader) {
	using namespace P3D::Graphics;

	Parser::Parse result = Parser::parse(code);
}