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

		const std::string* m_SourceName = nullptr;
		Messages* m_Messages = nullptr;
		std::string m_LineSource;
		std::size_t m_Line = 1,
					m_Column = 0;
		bool m_IsIdentifier = false;
		std::size_t m_IdentifierBegin = 0,
					m_IdentifierEnd = 0;
		bool m_IsComment = false;
		bool m_HasError = false;

	public:
		Lexer() noexcept = default;
		Lexer(Lexer&& lexer) noexcept;
		~Lexer() = default;

	public:
		Lexer& operator=(Lexer&& lexer) noexcept;

	public:
		void Clear() noexcept;
		bool IsEmpty() const noexcept;
		std::vector<Token> Tokens() noexcept;

		bool Lex(const std::string& sourceName, const std::string& source, Messages& messages);

	private:
		ISINLINE bool ReadDigits(std::size_t& end);
		ISINLINE bool ReadScientificNotation(std::size_t& end);
		bool ReadBinDigits(std::size_t& end);
		bool ReadOctDigits(std::size_t& end);
		bool ReadHexDigits(std::size_t& end);

		ISINLINE void LexInteger();
		ISINLINE void LexDecIntegerOrDecimal();
		ISINLINE void LexOtherIntegers();
		ISINLINE void LexStringOrCharacter(char quotation);
		ISINLINE bool LexSpecialCharacters();
		ISINLINE void LexPlus();
		ISINLINE void LexMinus();
		ISINLINE void LexMultiply();
		ISINLINE void LexDivide();
		ISINLINE void LexModulo();
		ISINLINE void LexAssign();
		ISINLINE void LexNot();
		ISINLINE void LexGreater();
		ISINLINE void LexLess();
		ISINLINE void LexBitAnd();
		ISINLINE void LexBitOr();
		ISINLINE void LexBitXor();
		ISINLINE void AddIdentifier();
	};
}