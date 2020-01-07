#pragma once

#include "../util/stringUtil.h"
#include <regex>

struct TokenType {

	enum Type : char {
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
		TYPE,
		NONE
	};

	static std::regex regexes[12];
	static Type types[12];

private:
	Type type = NONE;

public:
	TokenType() = default;
	TokenType(Type value) : type(type) {}

	operator Type() const { return type; }  
	bool operator==(Type other) const { return type == other; }
	bool operator==(TokenType other) const { return type == other.type; }
	bool operator!=(Type other) const { return type != other; }
	bool operator!=(TokenType other) const { return type != other.type; }
};

struct Token {
	TokenType type;
	std::string value;

	Token(const TokenType& type, const std::string& value) : type(type), value(value) {}
};

class Lexer {
private:
	static Token nextToken(std::string& input);
	static TokenType getMatch(const std::string& input);
	static Token popToken(std::string& input, TokenType type, std::string value);

public:
	static std::vector<Token> lex(std::string input);
};
