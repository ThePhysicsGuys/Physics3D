#include "core.h"

#include "shaderLexer.h"

TokenType::Type TokenType::types[12] = {
	ID,
	STRING,
	COMMENT,
	LP,
	RP,
	LB,
	RB,
	LC,
	RC,
	EOC,
	UNIFORM,
	TYPE
};

std::regex TokenType::regexes[12] = {
		std::regex("[A-Za-z_]\\w*"),
		std::regex("\"((?:\\\\.|[^\\\\\"])*)\""),
		std::regex("\\/\\*((?:.|[^\\*\\/])*)\\*\\/"),
		std::regex("\\("),
		std::regex("\\)"),
		std::regex("\\["),
		std::regex("\\]"),
		std::regex("\\{"),
		std::regex("\\}"),
		std::regex("\\;"),
		std::regex("uniform"),
		std::regex("mat2|mat3|mat4|float|int|vec2|vec3|vec4")
};

inline TokenType Lexer::getMatch(const std::string& input) {
	TokenType match = TokenType::NONE;

	for (int i = 0; i < 12; i++) {
		if (std::regex_match(input, TokenType::regexes[i]))
			match = TokenType::types[i];
	}

	return match;
}

inline Token Lexer::popToken(std::string& input, TokenType type, std::string value) {
	// Throw exception if the token is not recognized
	if (type == TokenType::NONE)
		Log::error("Type not recognized for value %s", value.c_str());

	// removes the token from the input
	input.erase(0, value.length());

	// removes all leading whitespaces from the input
	input = ltrim(input);

	// remove quotes from the value if the type is a string, regex or comment
	switch (type) {
		case TokenType::COMMENT:
			value = value.substr(1, value.length() - 1);
		case TokenType::STRING:
			value = value.substr(1, value.length() - 1);
			break;
	}

	return Token(type, value);
}

inline Token Lexer::nextToken(std::string& input) {
	std::string currentToken;

	TokenType lastMatch = TokenType::NONE;
	for (int i = 0; i < input.length(); i++) {
		currentToken.append(std::string(1, input.at(i)));

		// search for matches
		TokenType match = getMatch(currentToken);

		// No current match
		if (match == TokenType::NONE) {
			// Earlier match found
			if (lastMatch != TokenType::NONE) {
				// Remove last char
				currentToken.erase(currentToken.end() - 1);
				// break the loop
				break;
			}
			// else continue to search for a match
		} else {
			// update last match
			lastMatch = match;
		}
	}

	return popToken(input, lastMatch, currentToken);
}

inline std::vector<Token> Lexer::lex(std::string input) {
	std::vector<Token> tokens;

	input = trim(input);

	while (input.length() != 0) {
		Token token = nextToken(input);

		if (token.type == TokenType::COMMENT)
			continue;

		tokens.push_back(token);
	}

	tokens.push_back(Token(TokenType::EOC, ""));

	return tokens;
}