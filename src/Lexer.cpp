#include <ice/Lexer.hpp>

#include <ice/Encoding.hpp>
#include <ice/Utility.hpp>

#include <sstream>
#include <utility>

#ifdef _MSC_VER
#	pragma warning(disable: 4100)
#endif

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
	
	bool Lexer::Lex(const std::string& sourceName, const std::string& source, Messages& messages) {
		Clear();

		bool hasError = false;
		bool isIncomplete = false;

		std::size_t line = 1;
		std::size_t lineBegin = 0, nextLineBegin = source.find('\n');
		std::string lineSource;

		do {
			lineSource = source.substr(lineBegin, nextLineBegin - lineBegin);
			if (!lineSource.empty() && lineSource.back() == '\r') {
				lineSource.erase(lineSource.end() - 1);
			}

			char c;
			int cLength;
			for (std::size_t column = 0; column < lineSource.size(); column += cLength) {
				c = lineSource[column];
				cLength = GetCodepointLength(c);

#define lexingDatas sourceName, messages, lineSource, line, column, hasError, isIncomplete
				if (IsDigit(c)) {
					LexInteger(lexingDatas);
				} else if (IsWhitespace(c)) {
					LexWhitespace(lexingDatas);
				} else if (c == '\r') {
					messages.AddError("unexpected carriage return token", sourceName, line, column);
					hasError = true;
				}
			}

			++line;
		} while ((lineBegin = nextLineBegin + 1, nextLineBegin = source.find('\n', lineBegin), lineBegin) != 0);

		return !hasError && !isIncomplete;
	}

	namespace {
		ISINLINE bool ReadDigits(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
								 bool& hasError, bool& isIncomplete, std::size_t& end) {
			while (end < lineSource.size() &&
				  (IsDigit(lineSource[end]) || lineSource[end] == '\'')) ++end;
			if (lineSource[end - 1] == '\'') {
				messages.AddError("expected digit token after '\''", sourceName, line, end - 1,
								  CreateMessageNoteLocation(lineSource, line, end - 1, 1));
				column = end - 1;
				hasError = true;
				return true;
			}
			return false;
		}
		ISINLINE bool ReadScientificNotation(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
											 bool& hasError, bool& isIncomplete, std::size_t& end) {
			if (end == lineSource.size() || (lineSource[end] != 'e' && lineSource[end] != 'E')) {
				return false;
			}
			if (end + 1 < lineSource.size()) {
				if ((lineSource[++end] == '+' || lineSource[end] == '-') && end + 1 < lineSource.size()) {
					messages.AddError(Format("expected digit token after '%'", { std::string(1, lineSource[end + 1]) }),
									  sourceName, line, end + 1,
									  CreateMessageNoteLocation(lineSource, line, end + 1, 1));
					column = end - 1;
					hasError = true;
					return true;
				}
				if (ReadDigits(lexingDatas, ++end)) {
					return true;
				}
				return false;
			} else {
				messages.AddError(Format("expected digit token after '%'", { std::string(1, lineSource[end]) }),
								  sourceName, line, end,
								  CreateMessageNoteLocation(lineSource, line, end, 1));
				column = end - 1;
				hasError = true;
				return true;
			}
		}
		bool ReadBinDigits(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
						   bool& hasError, bool& isIncomplete, std::size_t& end) {
			bool hasErrorInIt = false;
			while (end < lineSource.size() &&
				  (IsDigit(lineSource[end]) || lineSource[end] == '\'')) {
				if ('1' < lineSource[end]) {
					messages.AddError(Format("invalid digit '%' in binary constant", { std::string(1, lineSource[end]) }),
									  sourceName, line, end,
									  CreateMessageNoteLocation(lineSource, line, end, 1));
					hasError = true;
					hasErrorInIt = true;
				}
				++end;
			}
			if (hasErrorInIt) {
				column = end - 1;
				return true;
			}
			if (lineSource[end - 1] == '\'') {
				messages.AddError("expected digit token after '\''", sourceName, line, end - 1,
								  CreateMessageNoteLocation(lineSource, line, end - 1, 1));
				column = end - 1;
				hasError = true;
				return true;
			}
			return false;
		}
		bool ReadOctDigits(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
						   bool& hasError, bool& isIncomplete, std::size_t& end) {
			bool hasErrorInIt = false;
			while (end < lineSource.size() &&
				  (IsDigit(lineSource[end]) || lineSource[end] == '\'')) {
				if ('7' < lineSource[end]) {
					messages.AddError(Format("invalid digit '%' in octal constant", { std::string(1, lineSource[end]) }),
									  sourceName, line, end,
									  CreateMessageNoteLocation(lineSource, line, end, 1));
					hasError = true;
					hasErrorInIt = true;
				}
				++end;
			}
			if (hasErrorInIt) {
				column = end - 1;
				return true;
			}
			if (lineSource[end - 1] == '\'') {
				messages.AddError("expected digit token after '\''", sourceName, line, end - 1,
								  CreateMessageNoteLocation(lineSource, line, end - 1, 1));
				column = end - 1;
				hasError = true;
				return true;
			}
			return false;
		}
		bool ReadHexDigits(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
						   bool& hasError, bool& isIncomplete, std::size_t& end) {
			while (end < lineSource.size() &&
				(IsDigit(lineSource[end]) ||
				('A' <= lineSource[end] && lineSource[end] <= 'F') ||
				('a' <= lineSource[end] && lineSource[end] <= 'f') ||
				lineSource[end] == '\'')) {
				++end;
			}
			if (lineSource[end - 1] == '\'') {
				messages.AddError("expected digit token after '\''", sourceName, line, end - 1,
					CreateMessageNoteLocation(lineSource, line, end - 1, 1));
				column = end - 1;
				hasError = true;
				return true;
			}
			return false;
		}
	}

	ISINLINE void Lexer::LexInteger(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
									bool& hasError, bool& isIncomplete) {
		if (lineSource[column] != '0') {
			LexDecIntegerOrDecimal(lexingDatas);
		} else {
			LexOtherIntegers(lexingDatas);
		}
	}
	ISINLINE void Lexer::LexDecIntegerOrDecimal(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
												bool& hasError, bool& isIncomplete) {
		std::size_t endColumn = column + 1;
		if (ReadDigits(lexingDatas, endColumn)) {
			hasError = true;
			return;
		}
		
		if (endColumn < lineSource.size() && lineSource[endColumn] == '.') {
			const std::size_t oldEndColumn = ++endColumn;
			if (ReadDigits(lexingDatas, endColumn)) {
				return;
			} else if (oldEndColumn == endColumn) {
				messages.AddError("expected digit token after '.'", sourceName, line, endColumn - 1,
								  CreateMessageNoteLocation(lineSource, line, endColumn - 1, 1));
				hasError = true;
				if (lineSource[endColumn] == 'e' || lineSource[endColumn] == 'E') ++endColumn;
				ReadDigits(lexingDatas, endColumn);
				column = endColumn - 1;
				return;
			} else if (ReadScientificNotation(lexingDatas, endColumn)) {
				return;
			}
			m_Tokens.push_back(Token(TokenType::Decimal, lineSource.substr(column, endColumn - column), line, column));
		} else {
			const std::size_t oldEndColumn = endColumn;
			if (ReadScientificNotation(lexingDatas, endColumn)) {
				hasError = true;
				return;
			}
			m_Tokens.push_back(Token(endColumn == oldEndColumn ? TokenType::DecInteger : TokenType::Decimal,
									 lineSource.substr(column, endColumn - column), line, column));
		}

		column = endColumn - 1;
	}
	ISINLINE void Lexer::LexOtherIntegers(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
										  bool& hasError, bool& isIncomplete) {
		if (column + 1 == lineSource.size()) {
			m_Tokens.push_back(Token(TokenType::DecInteger, "0", line, column));
			return;
		}

		std::size_t endColumn = column + 1;
		decltype(&ReadBinDigits) digitReader = nullptr;
		TokenType base;

		switch (lineSource[endColumn]) {
		case 'b':
		case 'B':
			digitReader = &ReadBinDigits;
			base = TokenType::BinInteger;
			break;

		case 'x':
		case 'X':
			digitReader = &ReadHexDigits;
			base = TokenType::HexInteger;
			break;

		default:
			if (IsDigit(lineSource[endColumn])) {
				digitReader = &ReadOctDigits;
				base = TokenType::OctInteger;
				--endColumn;
				break;
			} else {
				m_Tokens.push_back(Token(TokenType::DecInteger, "0", line, column));
				column = endColumn - 1;
				return;
			}
		}

		if (digitReader(lexingDatas, ++endColumn)) {
			return;
		}
		if (endColumn - 1 == column) {
			messages.AddError(Format("expected digit token after '%'", { std::string(1, lineSource[column + 1]) }),
							  sourceName, line, endColumn - 1,
							  CreateMessageNoteLocation(lineSource, line, endColumn - 1, 1));
			column = endColumn - 1;
			hasError = true;
			return;
		}

		if (endColumn < lineSource.size()) {
			const std::size_t oldEndColumn = endColumn;
			if (lineSource[endColumn] == '.') {
				ReadDigits(lexingDatas, ++endColumn);
			} else goto done;
			if (lineSource[endColumn] == 'e' || lineSource[endColumn] == 'E') {
				ReadScientificNotation(lexingDatas, endColumn);
			} else if (oldEndColumn == endColumn) goto done;

			messages.AddError(Format("invalid suffix '%' in integer constant", { lineSource.substr(oldEndColumn, endColumn - oldEndColumn) }),
							  sourceName, line, oldEndColumn,
							  CreateMessageNoteLocation(lineSource, line, oldEndColumn, endColumn - oldEndColumn));
			column = endColumn - 1;
			hasError = true;
		} else {
		done:
			m_Tokens.push_back(Token(base, lineSource.substr(column, endColumn - column), line, column));
			column = endColumn - 1;
		}
	}
	ISINLINE void Lexer::LexWhitespace(const std::string& sourceName, Messages& messages, const std::string& lineSource, std::size_t line, std::size_t& column,
									   bool& hasError, bool& isIncomplete) {
		if (!m_Tokens.empty() && m_Tokens.back().Type() == TokenType::Whitespace) {
			m_Tokens.back().Word(m_Tokens.back().Word() + lineSource[column]);
		} else {
			m_Tokens.push_back(Token(TokenType::Whitespace, std::string(1, lineSource[column]), line, column));
		}
	}
}