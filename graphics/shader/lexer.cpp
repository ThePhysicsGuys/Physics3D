#include "lexer.h"

namespace P3D::Graphics {
	constexpr bool Lexer::isDigit(char c) noexcept {
		return c >= '0' && c <= '9';
	}

	constexpr bool Lexer::isSpace(char c) noexcept {
		switch (c) {
			case ' ':
			case '\r':
			case '\n':
			case '\t':
				return true;
			default:
				return false;
		}
	}

	constexpr bool Lexer::isLetter(char c) noexcept {
		return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z';
	}

	constexpr bool Lexer::isIdentifierBody(char c) noexcept {
		return isIdentifierPrefix(c) || isDigit(c);
	}

	constexpr bool Lexer::isOperator(char c) noexcept {
		switch (c) {
			case '+':
			case '-':
			case '*':
			case '/':
			case '<':
			case '>':
			case '~':
			case '&':
			case '|':
			case '!':
			case '?':
				return true;
			default:
				return false;
		}
	}

	constexpr bool Lexer::isIdentifierPrefix(char c) noexcept {
		return isLetter(c) || c == '_';
	}

	constexpr Lexer::Token Lexer::lexNumber() noexcept {
		const std::size_t start = current;

		popChar();
		bool dot = false;
		while (isDigit(peekChar()) || peekChar() == '.') {
			if (peekChar() == '.') {
				if (dot)
					return Token(Token::Number, start, current);
				else
					dot = true;
			}
			popChar();
		}

		return Token(Token::Number, start, current);
	}

	constexpr Lexer::Token Lexer::lexIdentifier() noexcept {
		const std::size_t start = current;

		popChar();
		while (isIdentifierBody(peekChar()))
			popChar();

		std::string_view type = view(start, current);

		if (type == "true" || type == "false")
			return Token(Token::Boolean, start, current);
		
		if (type == "in" || type == "out")
			return Token(Token::InOut, start, current);

		if (type == "uniform")
			return Token(Token::Uniform, start, current);

		if (type == "const" || type == "smooth" || type == "flat")
			return Token(Token::Qualifier, start, current);

		if (type == "struct")
			return Token(Token::Struct, start, current);
		
		if (type == "layout")
			return Token(Token::Layout, start, current);

		if (type == "bool" || type == "mat2" || type == "mat3" || type == "mat4" || type == "float" || type == "int" || type == "vec2" || type == "vec3" || type == "vec4" || type == "VS_OUT" || type == "void" || type == "sampler2D" || type == "sampler3D" || type == "color3" || type == "color4")
			return Token(Token::Datatype, start, current);
		
		return Token(Token::Identifier, start, current);
	}

	constexpr Lexer::Token Lexer::lexSingleComment() noexcept {
		const std::size_t start = current;

		while (peekChar() != '\n' && peekChar() != '\0')
			popChar();

		return Token(Token::Comment, start, current);
	}

	constexpr Lexer::Token Lexer::lexMultiComment() noexcept {
		const std::size_t start = current;

		while (true) {
			switch (peekChar()) {
				case '\0':
					return Token(Token::Comment, start, current);
				case '*':
					switch (peekChar(1)) {
					case '\0':
						popChar();
						return Token(Token::Comment, start, current - 1);
					case '/':
						popChar();
						popChar();
						return Token(Token::Comment, start, current - 2);
					default:
						break;
					}
				default:
					popChar();
					break;
			}
		}
	}

	constexpr Lexer::Token Lexer::lexString(char type) noexcept {
		const std::size_t start = current;

		popChar();
		while (peekChar() != type)
			popChar();

		popChar();

		return Token(Token::String, start + 1, current - 1);
	}

	constexpr Lexer::Token Lexer::lexOperator() noexcept {
		const std::size_t start = current;

		popChar();
		while (isOperator(peekChar()))
			popChar();

		return Token(Token::Operator, start, current);
	}

	constexpr Lexer::Token Lexer::lexOperatorOrComment() noexcept {
		if (peekChar() == '/') {
			switch (peekChar(1)) {
			case '\0':
				return lexChar(Token::Operator);
			case '/':
				popChar();
				popChar();
				return lexSingleComment();
			case '*':
				popChar();
				popChar();
				return lexMultiComment();
			default:
				return lexOperator();
			}
		}

		return lexOperator();
	}

	constexpr Lexer::Token Lexer::lexVersionOrPreprocessor() noexcept {
		const auto start = current;

		popChar();
	
		if (isIdentifierPrefix(peekChar()))
			popChar();
		else
			return Token(Token::Error, start, current);

		while (isIdentifierBody(peekChar()))
			popChar();

		if (view(start, current) == "#version")
			return Token(Token::Version, start, current);

		return Token(Token::Preprocessor, start, current);
	}

	constexpr Lexer::Token Lexer::lexEnd() const noexcept {
		return Token(Token::End, current, current + 1);
	}

	constexpr Lexer::Token Lexer::lexChar(const Token::Type type) noexcept {
		std::size_t start = current;
		current++;
		
		return Token(type, start, current);
	}

	constexpr char Lexer::peekChar(std::size_t offset) const noexcept {
		return code[current + offset];
	}

	constexpr char Lexer::popChar() noexcept {
		return code[current++];
	}

	Lexer::Lexer(const char* code) noexcept : current(0), code(code) {

	}

	Lexer::Token Lexer::peek(std::size_t offset) noexcept {
		while (isSpace(peekChar()))
			popChar();
		
		std::size_t temp = current;
		
		Token token = next();
		for (std::size_t index = 0; index < offset; index++)
			token = next();
		
		current = temp;

		return token;
	}

	void Lexer::advance(std::size_t offset) noexcept {
		current += offset;
	}

	constexpr std::string_view Lexer::view(std::size_t start, std::size_t stop) noexcept {
		return std::string_view(this->code + start, stop - start);
	}

	std::string Lexer::string(std::size_t start, std::size_t stop) noexcept {
		return std::string(this->code + start, stop - start);
	}

	Lexer::Token Lexer::next() noexcept {
		while (isSpace(peekChar()))
			popChar();

		const char character = peekChar();

		if (character == 0)
			return Token(Token::End, current, current);

		if (isIdentifierPrefix(character))
			return lexIdentifier();

		if (isDigit(character))
			return lexNumber();

		if (isOperator(character))
			return lexOperatorOrComment();

		switch (character) {
			case '\0':
				return lexEnd();
			case '#':
				return lexVersionOrPreprocessor();
			case ':':
				return lexChar(Token::Colon);
			case '=':
				return lexChar(Token::Equals);
			case '(':
				return lexChar(Token::LeftParenthesis);
			case ')':
				return lexChar(Token::RightParenthesis);
			case '{':
				return lexChar(Token::LeftCurl);
			case '}':
				return lexChar(Token::RightCurl);
			case '[':
				return lexChar(Token::LeftBracket);
			case ']':
				return lexChar(Token::RightBracket);
			case '\'':
				return lexChar(Token::SingleQuote);
			case '"':
				return lexString('"');
			case '.':
				return lexChar(Token::Dot);
			case ',':
				return lexChar(Token::Comma);
			case ';':
				return lexChar(Token::Semicolon);
			default:
				return lexChar(Token::Error);
		}
	}
	
}
