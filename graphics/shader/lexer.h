#pragma once

#include <string_view>
#include <string>

namespace P3D::Graphics {
	class Lexer {
	public:
		struct Range {
			std::size_t start = 0;
			std::size_t stop = 0;

			[[nodiscard]] constexpr std::size_t size() const {
				return stop - start;
			}

			[[nodiscard]] constexpr const char* first(const char* code) const {
				return code + start;
			}

			[[nodiscard]] constexpr const char* last(const char* code) const {
				return code + stop;
			}

			[[nodiscard]] constexpr std::string_view view(const char* code) const {
				return std::string_view(this->first(code), this->size());
			}

			[[nodiscard]] std::string string(const char* code) const {
				return std::string(this->first(code), this->size());
			}
		};
		
		struct Token {
			enum Type : char {
				Error,
				Comma,
				Semicolon,
				LeftParenthesis,
				RightParenthesis,
				Equals,
				Dot,
				InOut,
				Layout,
				Uniform,
				Qualifier,
				Struct,
				Version,
				Preprocessor,
				Datatype,
				Identifier,
				LeftCurl,
				RightCurl,
				LeftBracket,
				RightBracket,
				Number,
				Operator,
				SingleQuote,
				DoubleQuote,
				String,
				Comment,
				End
			};

			Type type;
			Range range;

			constexpr Token(const Type type, std::size_t start, std::size_t stop) : type(type), range(Range { start, stop }) {}

			[[nodiscard]] constexpr std::size_t size() const {
				return this->range.size();
			}

			[[nodiscard]] constexpr std::string_view view(const char* code) const {
				return this->range.view(code);
			}

			[[nodiscard]] std::string string(const char* code) const {
				return this->range.string(code);
			}

			[[nodiscard]] constexpr const char* first(const char* code) const {
				return this->range.first(code);
			}

			[[nodiscard]] constexpr const char* last(const char* code) const {
				return this->range.last(code);
			}
		};
		
	private:
		std::size_t current;
		
		[[nodiscard]] constexpr static bool isDigit(char c) noexcept;
		[[nodiscard]] constexpr static bool isSpace(char c) noexcept;
		[[nodiscard]] constexpr static bool isLetter(char c) noexcept;
		[[nodiscard]] constexpr static bool isIdentifierPrefix(char c) noexcept;
		[[nodiscard]] constexpr static bool isIdentifierBody(char c) noexcept;
		[[nodiscard]] constexpr static bool isOperator(char c) noexcept;

		[[nodiscard]] constexpr Token lexString(char type) noexcept;
		[[nodiscard]] constexpr Token lexNumber() noexcept;
		[[nodiscard]] constexpr Token lexIdentifier() noexcept;
		[[nodiscard]] constexpr Token lexOperatorOrComment() noexcept;
		[[nodiscard]] constexpr Token lexVersionOrPreprocessor() noexcept;
		[[nodiscard]] constexpr Token lexSingleComment() noexcept;
		[[nodiscard]] constexpr Token lexMultiComment() noexcept;
		[[nodiscard]] constexpr Token lexOperator() noexcept;
		[[nodiscard]] constexpr Token lexChar(Token::Type type) noexcept;
		[[nodiscard]] constexpr Token lexEnd() const noexcept;

		[[nodiscard]] constexpr char peekChar(std::size_t offset = 0) const noexcept;
		constexpr char popChar() noexcept;

	public:
		const char* code;
		
		explicit Lexer(const char* code) noexcept;

		void advance(std::size_t offset) noexcept;

		[[nodiscard]] constexpr std::string_view view(std::size_t start, std::size_t stop) noexcept;
		[[nodiscard]] std::string string(std::size_t start, std::size_t stop) noexcept;
		[[nodiscard]] Token peek(std::size_t offset = 0) noexcept;
		[[nodiscard]] Token next() noexcept;
	};
}
