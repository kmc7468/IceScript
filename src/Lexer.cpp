#include <ice/Lexer.hpp>

#include <ice/Encoding.hpp>
#include <ice/Utility.hpp>

#include <sstream>
#include <utility>

namespace ice {
	const std::map<TokenType, std::string> Token::m_TypeNames = {
#define E(x) { TokenType:: x, #x }
#include <ice/detail/TokenType.txt>
#undef E
	};

	Token::Token(TokenType type, std::string word, std::size_t line, std::size_t column) noexcept
		: m_Type(type), m_Word(std::move(word)), m_Line(line), m_Column(column) {
	}
	Token::Token(const Token& token)
		: m_Type(token.m_Type), m_Word(token.m_Word), m_Line(token.m_Line), m_Column(token.m_Column) {
	}
	Token::Token(Token&& token) noexcept
		: m_Type(token.m_Type), m_Word(std::move(token.m_Word)), m_Line(token.m_Line), m_Column(token.m_Column) {
		token.m_Type = TokenType::None;
		token.m_Line = token.m_Column = 0;
	}

	Token& Token::operator=(const Token& token) {
		m_Type = token.m_Type;
		m_Word = token.m_Word;
		m_Line = token.m_Line;
		m_Column = token.m_Column;

		return *this;
	}
	Token& Token::operator=(Token&& token) noexcept {
		m_Type = token.m_Type;
		m_Word = std::move(token.m_Word);
		m_Line = token.m_Line;
		m_Column = token.m_Column;

		token.m_Type = TokenType::None;
		token.m_Line = token.m_Column = 0;

		return *this;
	}

	TokenType Token::Type() const noexcept {
		return m_Type;
	}
	void Token::Type(TokenType newType) noexcept {
		m_Type = newType;
	}
	std::string Token::Word() const {
		return m_Word;
	}
	void Token::Word(std::string newWord) noexcept {
		m_Word = std::move(newWord);
	}
	std::size_t Token::Line() const noexcept {
		return m_Line;
	}
	void Token::Line(std::size_t newLine) noexcept {
		m_Line = newLine;
	}
	std::size_t Token::Column() const noexcept {
		return m_Column;
	}
	void Token::Column(std::size_t newColumn) noexcept {
		m_Column = newColumn;
	}

	std::string Token::ToString() const {
		std::ostringstream oss;
		oss << m_Line << ':' << m_Column + 1 << ": " << m_TypeNames.at(m_Type) << "(from \"" << m_Word << "\")";
		return oss.str();
	}
}

namespace ice {
	const std::map<std::string, TokenType> Lexer::m_Keywords = {
		{ "module", TokenType::ModuleKeyword },
		{ "import", TokenType::ImportKeyword },

		{ "int8", TokenType::Int8Keyword },
		{ "int16", TokenType::Int16Keyword },
		{ "int32", TokenType::Int32Keyword },
		{ "int64", TokenType::Int64Keyword },
		{ "int128", TokenType::Int128Keyword },
		{ "intptr", TokenType::IntPtrKeyword },
		{ "uint8", TokenType::UInt8Keyword },
		{ "uint16", TokenType::UInt16Keyword },
		{ "uint32", TokenType::UInt32Keyword },
		{ "uint64", TokenType::UInt64Keyword },
		{ "uint128", TokenType::UInt128Keyword },
		{ "uintptr", TokenType::UIntPtrKeyword },
		{ "float32", TokenType::Float32Keyword },
		{ "float64", TokenType::Float64Keyword },
		{ "number", TokenType::NumberKeyword },
		{ "void", TokenType::VoidKeyword },
		{ "bool", TokenType::BoolKeyword },
		{ "true", TokenType::TrueKeyword },
		{ "false", TokenType::FalseKeyword },
		{ "char", TokenType::CharKeyword },
		{ "char8", TokenType::Char8Keyword },
		{ "string", TokenType::StringKeyword },
		{ "string8", TokenType::String8Keyword },
		{ "null", TokenType::NullKeyword },
		{ "any", TokenType::AnyKeyword },
		{ "object", TokenType::ObjectKeyword },

		{ "enum", TokenType::EnumKeyword },
		{ "class", TokenType::ClassKeyword },
		{ "this", TokenType::ThisKeyword },
		{ "super", TokenType::SuperKeyword },
		{ "virtual", TokenType::VirtualKeyword },
		{ "override", TokenType::OverrideKeyword },
		{ "sealed", TokenType::SealedKeyword },
		{ "pub", TokenType::PubKeyword },
		{ "priv", TokenType::PrivKeyword },
		{ "trait", TokenType::TraitKeyword },
		{ "is", TokenType::IsKeyword },
		{ "as", TokenType::AsKeyword },

		{ "function", TokenType::FunctionKeyword },
		{ "return", TokenType::ReturnKeyword },

		{ "if", TokenType::IfKeyword },
		{ "else", TokenType::ElseKeyword },
		{ "switch", TokenType::SwitchKeyword },
		{ "case", TokenType::CaseKeyword },
		{ "for", TokenType::ForKeyword },
		{ "while", TokenType::WhileKeyword },
		{ "do", TokenType::DoKeyword },
		{ "break", TokenType::BreakKeyword },
		{ "continue", TokenType::ContinueKeyword },

		{ "var", TokenType::VarKeyword },
		{ "let", TokenType::LetKeyword },
		{ "mut", TokenType::MutKeyword },
		{ "static", TokenType::StaticKeyword },
		{ "new", TokenType::NewKeyword },

		{ "throw", TokenType::ThrowKeyword },
		{ "try", TokenType::TryKeyword },
		{ "catch", TokenType::CatchKeyword },

		{ "sizeof", TokenType::SizeOfKeyword },
		{ "typeof", TokenType::TypeOfKeyword },
		{ "nameof", TokenType::NameOfKeyword },
	};

	Lexer::Lexer(Lexer&& lexer) noexcept
		: m_Tokens(std::move(lexer.m_Tokens)) {
	}

	Lexer& Lexer::operator=(Lexer&& lexer) noexcept {
		m_Tokens = std::move(lexer.m_Tokens);

		return *this;
	}

	void Lexer::Clear() noexcept {
		m_Tokens.clear();
	}
	bool Lexer::IsEmpty() const noexcept {
		return m_Tokens.empty();
	}
	std::vector<Token> Lexer::Tokens() const {
		return std::move(m_Tokens);
	}

	namespace {
		bool ReadDigitCharacters(Messages& messages, const std::string& sourceName, const std::string& lineSource,
								 std::size_t& end, std::size_t line, std::size_t& column) {
			while (end < lineSource.size() &&
				  (IsDigit(lineSource[end]) || lineSource[end] == '\'')) ++end;
			if (lineSource[end - 1] == '\'') {
				messages.AddError("expected digit token after '\''", sourceName, line, end - 1,
								  CreateMessageNoteLocation(lineSource, line, end - 1, 1));
				column = end - 1;
				return true;
			}
			return false;
		}
		bool ReadScientificNotation(Messages& messages, const std::string& sourceName, const std::string& lineSource,
			std::size_t& end, std::size_t line, std::size_t& column) {
			if (end >= lineSource.size() || (lineSource[end] != 'e' && lineSource[end] != 'E')) {
				return false;
			}
			if (end + 1 < lineSource.size() && (lineSource[end + 1] == '+' || lineSource[end + 1] == '-')) {
				if (end + 2 >= lineSource.size() || (!IsDigit(lineSource[end + 2]) && lineSource[end + 2] != '\'')) {
					messages.AddError(Format("expected digit token after '%'", { std::string(1, lineSource[end + 1]) }),
									  sourceName, line, end + 1,
									  CreateMessageNoteLocation(lineSource, line, end + 1, 1));
					column = end - 1;
					return true;
				}
				if (ReadDigitCharacters(messages, sourceName, lineSource, ++++end, line, column)) {
					return true;
				}
			} else {
				messages.AddError(Format("expected digit token, '+' or '-' after '%'", { std::string(1, lineSource[end]) }),
								  sourceName, line, end,
								  CreateMessageNoteLocation(lineSource, line, end, 1));
				column = end - 1;
				return true;
			}
			return false;
		}
	}

	bool Lexer::Lex(const std::string& sourceName, const std::string& source, Messages& messages) {
		Clear();

		bool hasError = false;
		bool isIncomplete = false;

		std::size_t line = 1;
		std::size_t lineBegin = 0, nextLineBegin = source.find('\n');

		do {
			std::string lineSource = source.substr(lineBegin, nextLineBegin - lineBegin);
			if (!lineSource.empty() && lineSource.back() == '\r') {
				lineSource.erase(lineSource.end() - 1);
			}

			char c;
			int cLength;

			for (std::size_t column = 0; column < lineSource.size(); column += cLength) {
				c = lineSource[column];
				cLength = GetCodepointLength(c);

				if (IsDigit(c)) {
					if (c == '0') {
						// BinInteger or OctInteger or DecInteger or HexInteger
						// TODO
					} else {
						// DecInteger or Decimal
						std::size_t endColumn = column + 1;
						if (ReadDigitCharacters(messages, sourceName, lineSource, endColumn, line, column)) {
							hasError = true;
							continue;
						}

						if (endColumn < lineSource.size() && lineSource[endColumn] == '.') {
							// Decimal
							if (ReadDigitCharacters(messages, sourceName, lineSource, ++endColumn, line, column) ||
								ReadScientificNotation(messages, sourceName, lineSource, ++endColumn, line, column)) {
								hasError = true;
								continue;
							}
							m_Tokens.push_back(Token(TokenType::Decimal, lineSource.substr(column, endColumn - column), line, column));
						} else {
							if (endColumn < lineSource.size() && (lineSource[endColumn] == 'e' || lineSource[endColumn] == 'E')) {
								// Decimal
								if (ReadScientificNotation(messages, sourceName, lineSource, endColumn, line, column)) {
									hasError = true;
									continue;
								}
								m_Tokens.push_back(Token(TokenType::Decimal, lineSource.substr(column, endColumn - column), line, column));
							} else {
								// DecInteger
								m_Tokens.push_back(Token(TokenType::DecInteger, lineSource.substr(column, endColumn - column), line, column));
							}
						}

						column = endColumn - 1;
					}
				} else if (IsWhitespace(c)) {
					// Whitespace
					if (!m_Tokens.empty() && m_Tokens.back().Type() == TokenType::Whitespace) {
						m_Tokens.back().Word(m_Tokens.back().Word() + c);
					} else {
						m_Tokens.push_back(Token(TokenType::Whitespace, std::string(1, c), line, column));
					}
				} else if (c == '\r') {
					// EOL
					messages.AddError("unexpected carriage return token", sourceName, line, column);
					hasError = true;
				}
			}

			++line;
		} while ((lineBegin = nextLineBegin + 1, nextLineBegin = source.find('\n', lineBegin), lineBegin) != 0);

		return !hasError && !isIncomplete;
	}
}