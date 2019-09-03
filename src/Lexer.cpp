#include <ice/Lexer.hpp>

#include <ice/Encoding.hpp>
#include <ice/Utility.hpp>

#include <algorithm>
#include <sstream>
#include <utility>

#ifdef _MSC_VER
#	pragma warning(disable: 4100)
#endif

namespace ice {
	const std::unordered_map<TokenType, std::string> Token::m_TypeNames = {
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
		oss << m_Line << ':' << m_Column + 1 << ": " << m_TypeNames.at(m_Type) << "(\"" << m_Word << "\")";
		return oss.str();
	}
}

namespace ice {
	const std::unordered_map<std::string, TokenType> Lexer::m_Keywords = {
#define EOrg(type1, type2, str) { str, TokenType::type1##type2 }
#define E(type, str) EOrg(type, Keyword, str)
		E(Module, "module"),
		E(Import, "import"),
		
		E(Int8, "int8"),
		E(Int16, "int16"),
		E(Int32, "int32"),
		E(Int64, "int64"),
		E(Int128, "int128"),
		E(IntPtr, "intptr"),
		E(UInt8, "uint8"),
		E(UInt16, "uint16"),
		E(UInt32, "uint32"),
		E(UInt64, "uint64"),
		E(UInt128, "uint128"),
		E(UIntPtr, "uintptr"),
		E(Float32, "float32"),
		E(Float64, "float64"),
		E(Number, "number"),
		E(Void, "void"),
		E(Bool, "bool"),
		E(True, "true"),
		E(False, "false"),
		E(Char, "char"),
		E(Char8, "char8"),
		E(String, "string"),
		E(String8, "string8"),
		E(Null, "null"),
		E(Any, "any"),
		E(Object, "object"),

		E(Enum, "enum"),
		E(Struct, "struct"),
		E(LowerSelf, "self"),
		E(UpperSelf, "Self"),
		E(Trait, "trait"),
		E(Impl, "impl"),
		E(Pub, "pub"),
		E(Priv, "priv"),
		E(Is, "is"),
		E(As, "as"),

		E(Function, "function"),
		E(Return, "return"),
		E(Operator, "operator"),

		E(If, "if"),
		E(Else, "else"),
		E(Switch, "switch"),
		E(Case, "case"),
		E(For, "for"),
		E(While, "while"),
		E(Do, "do"),
		E(Break, "break"),
		E(Continue, "continue"),

		E(Var, "var"),
		E(Let, "let"),
		E(Mut, "mut"),
		E(New, "new"),

		E(Throw, "throw"),
		E(Try, "try"),
		E(Catch, "catch"),
		E(Finally, "finally"),

		E(SizeOf, "sizeof"),
		E(TypeOf, "typeof"),
#undef EOrg
#undef E
	};
	const std::unordered_map<char, const std::array<TokenType, 5>> Lexer::m_Operators = {
		{ '+', { TokenType::Plus, TokenType::Increment, TokenType::PlusAssign } },
		{ '-', { TokenType::Minus, TokenType::Decrement, TokenType::MinusAssign, TokenType::None, TokenType::RightwardsArrow } },
		{ '*', { TokenType::Multiply, TokenType::Exponent, TokenType::MultiplyAssign, TokenType::ExponentAssign } },
		{ '/', { TokenType::Divide, TokenType::None, TokenType::DivideAssign } },
		{ '%', { TokenType::Modulo, TokenType::None, TokenType::ModuloAssign } },
		{ '=', { TokenType::Assign, TokenType::Equal, TokenType::Equal, TokenType::None, TokenType::RightwardsDoubleArrow } },
		{ '!', { TokenType::Not, TokenType::None, TokenType::NotEqual } },
		{ '>', { TokenType::Greater, TokenType::BitLeftShift, TokenType::GreaterEqual, TokenType::BitLeftShiftAssign } },
		{ '<', { TokenType::Less, TokenType::BitRightShift, TokenType::LessEqual, TokenType::BitRightShiftAssign } },
		{ '&', { TokenType::BitAnd, TokenType::And, TokenType::BitAndAssign } },
		{ '|', { TokenType::BitOr, TokenType::Or, TokenType::BitOrAssign } },
		{ '^', { TokenType::BitXor, TokenType::None, TokenType::BitXorAssign } },
		{ '~', { TokenType::BitNot } },
		{ '{', { TokenType::LeftBrace } },
		{ '}', { TokenType::RightBrace } },
		{ '(', { TokenType::LeftParen } },
		{ ')', { TokenType::RightParen } },
		{ '[', { TokenType::LeftBigParen } },
		{ ']', { TokenType::RightBigParen } },
		{ '.', { TokenType::Dot } },
		{ ',', { TokenType::Comma } },
		{ ';', { TokenType::Semicolon } },
		{ ':', { TokenType::Colon } },
		{ '?', { TokenType::Question } },
	};
	const std::unordered_map<TokenType, std::string> Lexer::m_OperatorWords = {
#define E(type, str) { TokenType:: type, str }
		E(Plus, "+"),
		E(Increment, "++"),
		E(PlusAssign, "+="),
		E(Minus, "-"),
		E(Decrement, "--"),
		E(MinusAssign, "-="),
		E(Multiply, "*"),
		E(MultiplyAssign, "*="),
		E(Divide, "/"),
		E(DivideAssign, "/="),
		E(Modulo, "%"),
		E(ModuloAssign, "%="),
		E(Exponent, "**"),
		E(ExponentAssign, "**="),

		E(Assign, "="),
		E(Equal, "=="),
		E(NotEqual, "!="),
		E(Greater, ">"),
		E(GreaterEqual, ">="),
		E(Less, "<"),
		E(LessEqual, "<="),
		E(And, "&&"),
		E(Or, "||"),
		E(Not, "!"),

		E(BitAnd, "&"),
		E(BitAndAssign, "&="),
		E(BitOr, "|"),
		E(BitOrAssign, "|="),
		E(BitXor, "^"),
		E(BitXorAssign, "^="),
		E(BitNot, "~"),
		E(BitLeftShift, "<<"),
		E(BitLeftShiftAssign, "<<="),
		E(BitRightShift, ">>"),
		E(BitRightShiftAssign, ">>="),

		E(RightwardsArrow, "->"),
		E(RightwardsDoubleArrow, "=>"),

		E(LeftBrace, "{"),
		E(RightBrace, "}"),
		E(LeftParen, "("),
		E(RightParen, ")"),
		E(LeftBigParen, "["),
		E(RightBigParen, "]"),

		E(Dot, "."),
		E(Comma, ","),
		E(Semicolon, ";"),
		E(Colon, ":"),
		E(Question, "?"),
#undef E
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
	std::vector<Token> Lexer::Tokens() noexcept {
		return std::move(m_Tokens);
	}

	bool Lexer::Lex(const std::string& sourceName, const std::string& source, Messages& messages) {
		Clear();

		m_SourceName = &sourceName;
		m_Messages = &messages;

		std::size_t m_LineBegin = 0;
		std::size_t nextLineBegin = source.find('\n');

		do {
			m_LineSource = source.substr(m_LineBegin, nextLineBegin - m_LineBegin);
			if (!m_LineSource.empty() && m_LineSource.back() == '\r') {
				m_LineSource.erase(m_LineSource.end() - 1);
			}
			
			for (m_Column = 0; m_Column < m_LineSource.size(); m_Column += m_CharLength) {
				m_Char = m_LineSource[m_Column];
				m_CharLength = GetCodepointLength(m_Char);
				if (!Next()) break;
			}

			AddIdentifier();
			if (!m_IsNoEOLToken) {
				m_Tokens.push_back(Token(TokenType::EOL, "", m_Line, m_LineSource.size()));
				m_IsNoEOLToken = false;
			}
			++m_Line;
			m_IsComment = false;
		} while ((m_LineBegin = nextLineBegin + 1,
				  nextLineBegin = source.find('\n', m_LineBegin),
				  m_LineBegin) != 0);

		const bool result = !m_HasError;
		m_Line = 1;
		m_IsIdentifier = false;
		m_IsNoEOLToken = false;
		m_HasError = false;

		return result;
	}

	ISINLINE bool Lexer::Next() {
		if (!m_IsIdentifier && IsDigit(m_Char)) {
			LexInteger();
		} else if (m_Char == '"' || m_Char == '\'') {
			AddIdentifier();
			LexStringOrCharacter(m_Char);
		} else if (IsWhitespace(m_Char)) {
			AddIdentifier();
		} else if (m_Char == '\\') {
			AddIdentifier();
			m_Messages->AddError("unexpected EOL", *m_SourceName, m_Line, m_Column,
								 CreateMessageNoteLocation(m_LineSource, m_Line, m_Column, 1));
			m_HasError = true;
		} else if (m_Char == '\r') {
			if (m_Column + 1 != m_LineSource.size()) {
				AddIdentifier();
				m_Messages->AddError("unexpected carriage return token", *m_SourceName, m_Line, m_Column);
				m_Messages->AddNote("is the EOL in this source file a CR?", *m_SourceName);
				m_HasError = true;
			} else {
				m_IsNoEOLToken = true;
			}
		} else {
			if (LexSpecialCharacters()) {
				switch (m_Char) {
				case '`':
				case '@':
				case '#':
				case '$':
					AddIdentifier();
					m_Messages->AddError("unexpected invalid token", *m_SourceName, m_Line, m_Column,
										 CreateMessageNoteLocation(m_LineSource, m_Line, m_Column, 1));
					m_HasError = true;
					return true;
				}

				if (m_IsIdentifier) {
					m_IdentifierEnd += m_CharLength;
				} else {
					m_IsIdentifier = true;
					m_IdentifierBegin = m_Column;
					m_IdentifierEnd = m_Column + m_CharLength;
				}
			} else if (AddIdentifier() && m_Tokens.size() > 1) {
				std::iter_swap(m_Tokens.end() - 1, m_Tokens.end() - 2);
			}
		}
		return !m_IsComment;
	}

	ISINLINE bool Lexer::ReadDigits(std::size_t& end, bool(*digitChecker1)(char), bool(*digitChecker2)(char), const char* base) {
		bool hasError = false;
		while (end < m_LineSource.size() &&
			  (digitChecker1(m_LineSource[end]) || m_LineSource[end] == '\'')) {
			if (!digitChecker2(m_LineSource[end])) {
				m_Messages->AddError(Format("invalid digit '%' in % constant", { std::string(1, m_LineSource[end]), base }),
									 *m_SourceName, m_Line, end,
									 CreateMessageNoteLocation(m_LineSource, m_Line, end, 1));
				hasError = true;
			} else if (m_LineSource[end - 1] == '\'' && m_LineSource[end] == '\'') {
				m_Messages->AddError("expected digit token after '\''", *m_SourceName, m_Line, end,
									 CreateMessageNoteLocation(m_LineSource, m_Line, end, 1));
				hasError = true;
			}
			++end;
		}
		if (hasError) {
			m_Column = end - 1;
			return m_HasError = true;
		} else if (m_LineSource[end - 1] == '\'') {
			m_Messages->AddError("expected digit token after '\''", *m_SourceName, m_Line, end - 1,
								 CreateMessageNoteLocation(m_LineSource, m_Line, end - 1, 1));
			m_Column = end - 1;
			return m_HasError = true;
		} else return false;
	}
	ISINLINE bool Lexer::ReadBinDigits(std::size_t& end) {
		return ReadDigits(end, IsDigit, [](char c) {
			return c <= '1';
		}, "binary");
	}
	ISINLINE bool Lexer::ReadOctDigits(std::size_t& end) {
		return ReadDigits(end, IsDigit, [](char c) {
			return c <= '7';
		}, "octal");
	}
	ISINLINE bool Lexer::ReadDecDigits(std::size_t& end) {
		return ReadDigits(end, IsDigit, [](char) {
			return true;
		}, nullptr);
	}
	ISINLINE bool Lexer::ReadHexDigits(std::size_t& end) {
		return ReadDigits(end, [](char c) {
			return IsDigit(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
		}, [](char) {
			return true;
		}, nullptr);
	}
	ISINLINE bool Lexer::ReadScientificNotation(std::size_t& end) {
		if (end == m_LineSource.size() || (m_LineSource[end] != 'e' && m_LineSource[end] != 'E')) return false;
		else if (end + 1 < m_LineSource.size()) {
			const std::size_t oldEnd = ++end;
			if (ReadDecDigits(end)) return true;
			else if (oldEnd == end) {
				if (m_LineSource[oldEnd] == '+' || m_LineSource[oldEnd] == '-') {
					return ReadDecDigits(++end);
				} else {
					m_Messages->AddError(Format("expected digit token after '%'", { std::string(1, m_LineSource[oldEnd]) }),
										 *m_SourceName, m_Line, oldEnd,
										 CreateMessageNoteLocation(m_LineSource, m_Line, oldEnd, 1));
					m_Column = end - 1;
					return m_HasError = true;
				}
			} else return false;
		} else {
			m_Messages->AddError(Format("expected digit token after '%'", { std::string(1, m_LineSource[end]) }),
								 *m_SourceName, m_Line, end,
								 CreateMessageNoteLocation(m_LineSource, m_Line, end, 1));
			m_Column = end - 1;
			return m_HasError = true;
		}
	}
	
	ISINLINE void Lexer::LexInteger() {
		if (m_LineSource[m_Column] != '0') {
			LexDecIntegerOrDecimal();
		} else {
			LexOtherIntegers();
		}
	}
	ISINLINE void Lexer::LexDecIntegerOrDecimal() {
		std::size_t endColumn = m_Column + 1;
		if (ReadDecDigits(endColumn)) return;
		else if (endColumn < m_LineSource.size() && m_LineSource[endColumn] == '.') {
			const std::size_t oldEndColumn = ++endColumn;
			if (ReadDecDigits(endColumn)) return;
			else if (oldEndColumn == endColumn) {
				m_Messages->AddError("expected digit token after '.'", *m_SourceName, m_Line, endColumn - 1,
									 CreateMessageNoteLocation(m_LineSource, m_Line, endColumn - 1, 1));
				m_HasError = true;
				if (m_LineSource[endColumn] == 'e' || m_LineSource[endColumn] == 'E') {
					++endColumn;
					ReadDecDigits(endColumn);
				}
			} else if (!ReadScientificNotation(endColumn)) {
				m_Tokens.push_back(Token(TokenType::Decimal, m_LineSource.substr(m_Column, endColumn - m_Column), m_Line, m_Column));
			}
		} else {
			const std::size_t oldEndColumn = endColumn;
			if (!ReadScientificNotation(endColumn)) {
				m_Tokens.push_back(Token(endColumn == oldEndColumn ? TokenType::DecInteger : TokenType::Decimal,
								   m_LineSource.substr(m_Column, endColumn - m_Column), m_Line, m_Column));
			}
		}
		m_Column = endColumn - 1;
	}
	ISINLINE void Lexer::LexOtherIntegers() {
		if (m_Column + 1 == m_LineSource.size()) {
			m_Tokens.push_back(Token(TokenType::DecInteger, "0", m_Line, m_Column));
			return;
		}

		std::size_t endColumn = m_Column + 1;
		bool(Lexer::*digitReader)(std::size_t&) = nullptr;
		TokenType base;
		
		switch (m_LineSource[endColumn]) {
		case 'b':
		case 'B':
			digitReader = &Lexer::ReadBinDigits;
			base = TokenType::BinInteger;
			break;

		case 'x':
		case 'X':
			digitReader = &Lexer::ReadHexDigits;
			base = TokenType::HexInteger;
			break;

		default:
			ReadDecDigits(endColumn);
			if (endColumn < m_LineSource.size() &&
			   (m_LineSource[endColumn] == '.' || m_LineSource[endColumn] == 'e' || m_LineSource[endColumn] == 'E')) {
				LexDecIntegerOrDecimal();
				return;
			} else {
				digitReader = &Lexer::ReadOctDigits;
				base = TokenType::OctInteger;
				--endColumn;
				break;
			}
		}

		if ((this->*digitReader)(++endColumn)) return;
		else if (endColumn < m_LineSource.size()) {
			const std::size_t oldEndColumn = endColumn;
			if (m_LineSource[endColumn] == '.') {
				ReadDecDigits(++endColumn);
			} else goto done;
			if (m_LineSource[endColumn] == 'e' || m_LineSource[endColumn] == 'E') {
				ReadScientificNotation(endColumn);
			} else if (oldEndColumn == endColumn) goto done;

			m_Messages->AddError(Format("invalid suffix '%' in integer constant", { m_LineSource.substr(oldEndColumn, endColumn - oldEndColumn) }),
								 *m_SourceName, m_Line, oldEndColumn,
								 CreateMessageNoteLocation(m_LineSource, m_Line, oldEndColumn, endColumn - oldEndColumn));
			m_Column = endColumn - 1;
			m_HasError = true;
		} else {
		done:
			m_Tokens.push_back(Token(base, m_LineSource.substr(m_Column, endColumn - m_Column), m_Line, m_Column));
			m_Column = endColumn - 1;
		}
	}
	ISINLINE void Lexer::LexStringOrCharacter(char quotation) {
		std::size_t endColumn = m_Column + 1;
		do {
			while (endColumn < m_LineSource.size() && m_LineSource[endColumn] != quotation) ++endColumn;
			if (endColumn == m_LineSource.size()) {
				m_Messages->AddError("unexcpeted EOL", *m_SourceName, m_Line, endColumn - 1,
									 CreateMessageNoteLocation(m_LineSource, m_Line, endColumn - 1, 1));
				m_Column = endColumn - 1;
				m_HasError = true;
				return;
			}
			++endColumn;
		} while (m_LineSource[endColumn - 2] == '\\');

		m_Tokens.push_back(Token(quotation == '"' ? TokenType::String : TokenType::Character, m_LineSource.substr(m_Column, endColumn - m_Column), m_Line, m_Column));
		m_Column = endColumn - 1;
	}
	ISINLINE bool Lexer::LexSpecialCharacters() {
		const auto iter = m_Operators.find(m_Char);
		if (iter == m_Operators.end()) return true;

		int index = 0;
		std::size_t column = m_Column + 1;
		if (column < m_LineSource.size()) {
			const char nextChar = m_LineSource[column];
			if (m_Char == nextChar) {
				if (++column < m_LineSource.size() && m_LineSource[column] == '=' &&
					iter->second[3] != TokenType::None) {
					index = 3;
					++column;
				} else if (iter->second[1] != TokenType::None) {
					index = 1;
				}
			} else if (nextChar == '=' && iter->second[2] != TokenType::None) ++column, index = 2;
			  else if (nextChar == '>' && iter->second[4] != TokenType::None) ++column, index = 4;
		}

		m_Tokens.push_back(Token(iter->second[index], m_OperatorWords.at(iter->second[index]), m_Line, m_Column));
		m_Column = column - 1;
		return false;
	}
	ISINLINE bool Lexer::AddIdentifier() {
		if (!m_IsIdentifier) return false;

		m_IsIdentifier = false;
		m_Tokens.push_back(Token(TokenType::Identifer, m_LineSource.substr(m_IdentifierBegin, m_IdentifierEnd - m_IdentifierBegin),
								 m_Line, m_IdentifierBegin));
		if (auto iter = m_Keywords.find(m_Tokens.back().Word()); iter != m_Keywords.end()) {
			m_Tokens.back().Type(iter->second);
		}
		return true;
	}
}