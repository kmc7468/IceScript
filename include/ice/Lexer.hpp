#pragma once

#include <ice/Message.hpp>

#include <cstddef>
#include <map>
#include <string>
#include <vector>

namespace ice {
	enum class TokenType {
#define E(x) x
#include <ice/detail/TokenType.txt>
#undef E
	};

	class Token final {
	private:
		static const std::map<TokenType, std::string> m_TypeNames;

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
		static const std::map<std::string, TokenType> m_Keywords;

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
	};
}