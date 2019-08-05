#pragma once

#include <ice/Message.hpp>
#include <ice/detail/Config.hpp>

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace ice {
	enum class TokenType {
#define E(x) x
#include <ice/detail/TokenType.txt>
#undef E
	};

	class Token final {
	private:
		static const std::unordered_map<TokenType, std::string> m_TypeNames;

	private:
		TokenType m_Type = TokenType::None;
		std::string m_Word;
		std::size_t m_Line = 0, m_Column = 0;

	public:
		Token() noexcept = default;
		Token(TokenType type, std::string word, std::size_t line, std::size_t column) noexcept;
		Token(const Token& token);
		Token(Token&& token) noexcept;
		~Token() = default;

	public:
		Token& operator=(const Token& token);
		Token& operator=(Token&& token) noexcept;

	public:
		TokenType Type() const noexcept;
		void Type(TokenType newType) noexcept;
		std::string Word() const;
		void Word(std::string newWord) noexcept;
		std::size_t Line() const noexcept;
		void Line(std::size_t newLine) noexcept;
		std::size_t Column() const noexcept;
		void Column(std::size_t newColumn) noexcept;

	public:
		std::string ToString() const;
	};

	class Lexer final {
	private:
		static const std::unordered_map<std::string, TokenType> m_Keywords;

	private:
		std::vector<Token> m_Tokens;

	public:
		Lexer() noexcept = default;
		Lexer(Lexer&& lexer) noexcept;
		~Lexer() = default;

	public:
		Lexer& operator=(Lexer&& lexer) noexcept;

	public:
		void Clear() noexcept;
		bool IsEmpty() const noexcept;
		std::vector<Token> Tokens() const;

		bool Lex(const std::string& sourceName, const std::string& source, Messages& messages);

	private:
		ISINLINE void LexInteger(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
								 bool& hasError, bool& isIncomplete);
		ISINLINE void LexDecIntegerOrDecimal(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
											 bool& hasError, bool& isIncomplete);
		ISINLINE void LexOtherIntegers(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
									   bool& hasError, bool& isIncomplete);
		ISINLINE void LexStringOrCharacter(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
										   bool& hasError, bool& isIncomplete, char quotation);
		ISINLINE void LexWhitespace(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
									bool& hasError, bool& isIncomplete);

		ISINLINE bool LexSpecialCharacters(const std::string& lineSource, std::size_t line, std::size_t& column, bool& isComment);
		ISINLINE void LexPlus(const std::string& lineSource, std::size_t line, std::size_t& column);
		ISINLINE void LexMinus(const std::string& lineSource, std::size_t line, std::size_t& column);
		ISINLINE void LexMultiply(const std::string& lineSource, std::size_t line, std::size_t& column);
		ISINLINE void LexDivide(const std::string& lineSource, std::size_t line, std::size_t& column, bool& isComment);
		ISINLINE void LexModulo(const std::string& lineSource, std::size_t line, std::size_t& column);
		ISINLINE void LexAssign(const std::string& lineSource, std::size_t line, std::size_t& column);
		ISINLINE void LexNot(const std::string& lineSource, std::size_t line, std::size_t& column);
		ISINLINE void LexGreater(const std::string& lineSource, std::size_t line, std::size_t& column);
		ISINLINE void LexLess(const std::string& lineSource, std::size_t line, std::size_t& column);
		ISINLINE void LexBitAnd(const std::string& lineSource, std::size_t line, std::size_t& column);
		ISINLINE void LexBitOr(const std::string& lineSource, std::size_t line, std::size_t& column);
		ISINLINE void LexBitXor(const std::string& lineSource, std::size_t line, std::size_t& column);
	};
}