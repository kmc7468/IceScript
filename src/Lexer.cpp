#include <ice/Lexer.hpp>

#include <ice/Encoding.hpp>
#include <ice/Utility.hpp>

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
		{ "struct", TokenType::StructKeyword },
		{ "self", TokenType::LowerSelfKeyword },
		{ "Self", TokenType::UpperSelfKeyword },
		{ "trait", TokenType::TraitKeyword },
		{ "impl", TokenType::ImplKeyword },
		{ "pub", TokenType::PubKeyword },
		{ "priv", TokenType::PrivKeyword },
		{ "is", TokenType::IsKeyword },
		{ "as", TokenType::AsKeyword },

		{ "function", TokenType::FunctionKeyword },
		{ "return", TokenType::ReturnKeyword },
		{ "operator", TokenType::OperatorKeyword },

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
	std::vector<Token> Lexer::Tokens() noexcept {
		return std::move(m_Tokens);
	}

	bool Lexer::Lex(const std::string& sourceName, const std::string& source, Messages& messages) {
		Clear();

		m_SourceName = &sourceName;
		m_Messages = &messages;

		bool isNoEOLToken = false;
		std::size_t m_LineBegin = 0;
		std::size_t nextLineBegin = source.find('\n');

		do {
			m_LineSource = source.substr(m_LineBegin, nextLineBegin - m_LineBegin);
			if (!m_LineSource.empty() && m_LineSource.back() == '\r') {
				m_LineSource.erase(m_LineSource.end() - 1);
			}

			char c;
			int cLength;
			for (m_Column = 0; m_Column < m_LineSource.size(); m_Column += cLength) {
				c = m_LineSource[m_Column];
				cLength = GetCodepointLength(c);

				if (!m_IsIdentifier && IsDigit(c)) {
					LexInteger();
				} else if (c == '"' || c == '\'') {
					AddIdentifier();
					LexStringOrCharacter(c);
				} else if (IsWhitespace(c)) {
					AddIdentifier();
				} else if (c == '\r') {
					AddIdentifier();
					m_Messages->AddError("expected EOL", *m_SourceName, m_Line, m_Column,
										 CreateMessageNoteLocation(m_LineSource, m_Line, m_Column, 1));
					m_HasError = true;
				} else if (c == '\\') {
					if (m_Column + 1 != m_LineSource.size()) {
						AddIdentifier();
						m_Messages->AddError("unexpected carriage return token", *m_SourceName, m_Line, m_Column);
						m_HasError = true;
					} else {
						isNoEOLToken = true;
					}
				} else {
					if (LexSpecialCharacters()) {
						switch (c) {
						case '`':
						case '@':
						case '#':
						case '$':
							m_HasError = true;
							AddIdentifier();
							continue;
						}

						if (m_IsIdentifier) {
							m_IdentifierEnd += cLength;
						} else {
							m_IsIdentifier = true;
							m_IdentifierBegin = m_Column;
							m_IdentifierEnd = m_Column + cLength;
						}
					} else {
						AddIdentifier();
						if (m_IsComment) goto newLine;
					}
				}
			}

		newLine:
			AddIdentifier();
			if (!isNoEOLToken) {
				m_Tokens.push_back(Token(TokenType::EOL, "", m_Line, m_LineSource.size()));
				isNoEOLToken = false;
			}
			++m_Line;
			m_IsComment = false;
		} while ((m_LineBegin = nextLineBegin + 1,
				  nextLineBegin = source.find('\n', m_LineBegin),
				  m_LineBegin) != 0);

		const bool result = !m_HasError;
		m_Line = 1;
		m_IsIdentifier = false;
		m_HasError = false;

		return !result;
	}

	ISINLINE bool Lexer::ReadDigits(std::size_t& end) {
		while (end < m_LineSource.size() &&
			  (IsDigit(m_LineSource[end]) || m_LineSource[end] == '\'')) ++end;
		if (m_LineSource[end - 1] == '\'') {
			m_Messages->AddError("expected digit token after '\''", *m_SourceName, m_Line, end - 1,
								 CreateMessageNoteLocation(m_LineSource, m_Line, end - 1, 1));
			m_Column = end - 1;
			return m_HasError = true;
		} else return false;
	}
	ISINLINE bool Lexer::ReadScientificNotation(std::size_t& end) {
		if (end == m_LineSource.size() || (m_LineSource[end] != 'e' && m_LineSource[end] != 'E')) return false;
		else if (end + 1 < m_LineSource.size()) {
			const std::size_t oldEnd = ++end;
			if (ReadDigits(end)) return true;
			else if (oldEnd == end) {
				if (m_LineSource[oldEnd] == '+' || m_LineSource[oldEnd] == '-') {
					return ReadDigits(++end);
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
	bool Lexer::ReadBinDigits(std::size_t& end) {
		bool hasError = false;
		while (end < m_LineSource.size() &&
			  (IsDigit(m_LineSource[end]) || m_LineSource[end] == '\'')) {
			if ('1' < m_LineSource[end]) {
				m_Messages->AddError(Format("invalid digit '%' in binary constant", { std::string(1, m_LineSource[end]) }),
									 *m_SourceName, m_Line, end,
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
	bool Lexer::ReadOctDigits(std::size_t& end) {
		bool hasError = false;
		while (end < m_LineSource.size() &&
			(IsDigit(m_LineSource[end]) || m_LineSource[end] == '\'')) {
			if ('7' < m_LineSource[end]) {
				m_Messages->AddError(Format("invalid digit '%' in octal constant", { std::string(1, m_LineSource[end]) }),
									 *m_SourceName, m_Line, end,
									 CreateMessageNoteLocation(m_LineSource, m_Line, end, 1));
				hasError = true;
			}
			++end;
		}
		if (hasError) {
			m_Column = end - 1;
			return m_HasError = true;
		}
		else if (m_LineSource[end - 1] == '\'') {
			m_Messages->AddError("expected digit token after '\''", *m_SourceName, m_Line, end - 1,
								 CreateMessageNoteLocation(m_LineSource, m_Line, end - 1, 1));
			m_Column = end - 1;
			return m_HasError = true;
		}
		else return false;
	}
	bool Lexer::ReadHexDigits(std::size_t& end) {
		while (end < m_LineSource.size() &&
			  (IsDigit(m_LineSource[end]) ||
			  ('A' <= m_LineSource[end] && m_LineSource[end] <= 'F') ||
			  ('a' <= m_LineSource[end] && m_LineSource[end] <= 'f') ||
				  m_LineSource[end] == '\'')) ++end;
		if (m_LineSource[end - 1] == '\'') {
			m_Messages->AddError("expected digit token after '\''", *m_SourceName, m_Line, end - 1,
								 CreateMessageNoteLocation(m_LineSource, m_Line, end - 1, 1));
			m_Column = end - 1;
			return m_HasError = true;
		}
		else return false;
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
		if (ReadDigits(endColumn)) return;
		else if (endColumn < m_LineSource.size() && m_LineSource[endColumn] == '.') {
			const std::size_t oldEndColumn = ++endColumn;
			if (ReadDigits(endColumn)) return;
			else if (oldEndColumn == endColumn) {
				m_Messages->AddError("expected digit token after '.'", *m_SourceName, m_Line, endColumn - 1,
									 CreateMessageNoteLocation(m_LineSource, m_Line, endColumn - 1, 1));
				m_HasError = true;
				if (m_LineSource[endColumn] == 'e' || m_LineSource[endColumn] == 'E') {
					++endColumn;
					ReadDigits(endColumn);
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
		zero:
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
			if (IsDigit(m_LineSource[endColumn])) {
				digitReader = &Lexer::ReadOctDigits;
				base = TokenType::OctInteger;
				--endColumn;
				break;
			} else goto zero;
		}

		if ((this->*digitReader)(++endColumn)) return;
		else if (endColumn < m_LineSource.size()) {
			const std::size_t oldEndColumn = endColumn;
			if (m_LineSource[endColumn] == '.') {
				ReadDigits(++endColumn);
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
		switch (m_LineSource[m_Column]) {
#define caseOrg(value, function) case value: function(); break
#define case(value, function) caseOrg(value, function)
		case('+', LexPlus);
		case('-', LexMinus);
		case('*', LexMultiply);
#undef case
		case '/':
			LexDivide();
			break;
#define case(value, function) caseOrg(value, function)
		case('%', LexModulo);
		case('=', LexAssign);
		case('!', LexNot);
		case('>', LexGreater);
		case('<', LexLess);
		case('&', LexBitAnd);
		case('|', LexBitOr);
		case('^', LexBitXor);
#undef caseOrg
#undef case
#define caseOrg(value, tokenType) case value: m_Tokens.push_back(Token(TokenType::tokenType, std::string(1, value), m_Line, m_Column)); break
#define case(value, tokenType) caseOrg(value, tokenType)
		case('~', BitNot);
		case('{', LeftBrace);
		case('}', RightBrace);
		case('(', LeftParen);
		case(')', RightParen);
		case('[', LeftBigParen);
		case(']', RightBigParen);
		case('.', Dot);
		case(',', Comma);
		case(';', Semicolon);
		case(':', Colon);
		case('?', Question);
#undef case
		default:
			return true;
		}
		return false;
	}
	ISINLINE void Lexer::LexPlus() {
		if (m_Column + 1 < m_LineSource.size()) {
			switch (m_LineSource[m_Column + 1]) {
			case '+':
				m_Tokens.push_back(Token(TokenType::Increment, "++", m_Line, m_Column));
				break;

			case '=':
				m_Tokens.push_back(Token(TokenType::PlusAssign, "+=", m_Line, m_Column));
				break;

			default: goto plus;
			}
			++m_Column;
		} else {
		plus:
			m_Tokens.push_back(Token(TokenType::Plus, "+", m_Line, m_Column));
		}
	}
	ISINLINE void Lexer::LexMinus() {
		if (m_Column + 1 < m_LineSource.size()) {
			switch (m_LineSource[m_Column + 1]) {
			case '-':
				m_Tokens.push_back(Token(TokenType::Decrement, "--", m_Line, m_Column));
				break;

			case '=':
				m_Tokens.push_back(Token(TokenType::MinusAssign, "-=", m_Line, m_Column));
				break;

			case '>':
				m_Tokens.push_back(Token(TokenType::RightwardsArrow, "->", m_Line, m_Column));
				break;

			default: goto minus;
			}
			++m_Column;
		} else {
		minus:
			m_Tokens.push_back(Token(TokenType::Minus, "-", m_Line, m_Column));
		}
	}
	ISINLINE void Lexer::LexMultiply() {
		if (m_Column + 1 < m_LineSource.size()) {
			switch (m_LineSource[m_Column + 1]) {
			case '*':
				if (m_Column + 2 < m_LineSource.size() && m_LineSource[m_Column + 2] == '=') {
					m_Tokens.push_back(Token(TokenType::ExponentAssign, "**=", m_Line, m_Column));
					++m_Column;
				} else {
					m_Tokens.push_back(Token(TokenType::Exponent, "**", m_Line, m_Column));
				}
				break;

			case '=':
				m_Tokens.push_back(Token(TokenType::MultiplyAssign, "*=", m_Line, m_Column));
				break;

			default: goto multiply;
			}
			++m_Column;
		} else {
		multiply:
			m_Tokens.push_back(Token(TokenType::Multiply, "*", m_Line, m_Column));
		}
	}
	ISINLINE void Lexer::LexDivide() {
		if (m_Column + 1 < m_LineSource.size()) {
			switch (m_LineSource[m_Column + 1]) {
			case '/':
				m_IsComment = true;
				break;

			case '=':
				m_Tokens.push_back(Token(TokenType::DivideAssign, "/=", m_Line, m_Column));
				break;

			default: goto divide;
			}
			++m_Column;
		} else {
		divide:
			m_Tokens.push_back(Token(TokenType::Divide, "/", m_Line, m_Column));
		}
	}
	ISINLINE void Lexer::LexModulo() {
		if (m_Column + 1 < m_LineSource.size()) {
			switch (m_LineSource[m_Column + 1]) {
			case '=':
				m_Tokens.push_back(Token(TokenType::ModuloAssign, "%=", m_Line, m_Column));
				break;

			default: goto modulo;
			}
			++m_Column;
		} else {
		modulo:
			m_Tokens.push_back(Token(TokenType::Modulo, "%", m_Line, m_Column));
		}
	}
	ISINLINE void Lexer::LexAssign() {
		if (m_Column + 1 < m_LineSource.size()) {
			switch (m_LineSource[m_Column + 1]) {
			case '=':
				m_Tokens.push_back(Token(TokenType::Equal, "==", m_Line, m_Column));
				break;

			case '>':
				m_Tokens.push_back(Token(TokenType::RightwardsDoubleArrow, "=>", m_Line, m_Column));
				break;

			default: goto assign;
			}
			++m_Column;
		} else {
		assign:
			m_Tokens.push_back(Token(TokenType::Assign, "=", m_Line, m_Column));
		}
	}
	ISINLINE void Lexer::LexNot() {
		if (m_Column + 1 < m_LineSource.size()) {
			switch (m_LineSource[m_Column + 1]) {
			case '=':
				m_Tokens.push_back(Token(TokenType::NotEqual, "!=", m_Line, m_Column));
				break;

			default: goto not2;
			}
			++m_Column;
		} else {
		not2:
			m_Tokens.push_back(Token(TokenType::Not, "!", m_Line, m_Column));
		}
	}
	ISINLINE void Lexer::LexGreater() {
		if (m_Column + 1 < m_LineSource.size()) {
			switch (m_LineSource[m_Column + 1]) {
			case '=':
				m_Tokens.push_back(Token(TokenType::GreaterEqaul, ">=", m_Line, m_Column));
				break;

			case '>':
				if (m_Column + 2 < m_LineSource.size() && m_LineSource[m_Column + 2] == '=') {
					m_Tokens.push_back(Token(TokenType::BitRightShiftAssign, ">>=", m_Line, m_Column));
					++m_Column;
				} else {
					m_Tokens.push_back(Token(TokenType::BitRightShift, ">>", m_Line, m_Column));
				}
				break;

			default: goto greater;
			}
			++m_Column;
		} else {
		greater:
			m_Tokens.push_back(Token(TokenType::Greater, ">", m_Line, m_Column));
		}
	}
	ISINLINE void Lexer::LexLess() {
		if (m_Column + 1 < m_LineSource.size()) {
			switch (m_LineSource[m_Column + 1]) {
			case '=':
				m_Tokens.push_back(Token(TokenType::LessEqual, "<=", m_Line, m_Column));
				break;

			case '<':
				if (m_Column + 2 < m_LineSource.size() && m_LineSource[m_Column + 2] == '=') {
					m_Tokens.push_back(Token(TokenType::BitLeftShiftAssign, "<<=", m_Line, m_Column));
					++m_Column;
				} else {
					m_Tokens.push_back(Token(TokenType::BitLeftShift, "<<", m_Line, m_Column));
				}
				break;

			default: goto less;
			}
			++m_Column;
		} else {
		less:
			m_Tokens.push_back(Token(TokenType::Less, "<", m_Line, m_Column));
		}
	}
	ISINLINE void Lexer::LexBitAnd() {
		if (m_Column + 1 < m_LineSource.size()) {
			switch (m_LineSource[m_Column + 1]) {
			case '=':
				m_Tokens.push_back(Token(TokenType::BitAndAssign, "&=", m_Line, m_Column));
				break;

			case '&':
				m_Tokens.push_back(Token(TokenType::And, "&&", m_Line, m_Column));
				break;

			default: goto bitand2;
			}
			++m_Column;
		} else {
		bitand2:
			m_Tokens.push_back(Token(TokenType::BitAnd, "&", m_Line, m_Column));
		}
	}
	ISINLINE void Lexer::LexBitOr() {
		if (m_Column + 1 < m_LineSource.size()) {
			switch (m_LineSource[m_Column + 1]) {
			case '=':
				m_Tokens.push_back(Token(TokenType::BitOrAssign, "|=", m_Line, m_Column));
				break;

			case '|':
				m_Tokens.push_back(Token(TokenType::Or, "||", m_Line, m_Column));
				break;

			default: goto bitor2;
			}
			++m_Column;
		} else {
		bitor2:
			m_Tokens.push_back(Token(TokenType::BitOr, "|", m_Line, m_Column));
		}
	}
	ISINLINE void Lexer::LexBitXor() {
		if (m_Column + 1 < m_LineSource.size()) {
			switch (m_LineSource[m_Column + 1]) {
			case '=':
				m_Tokens.push_back(Token(TokenType::BitXorAssign, "^=", m_Line, m_Column));
				break;

			default: goto bitxor2;
			}
			++m_Column;
		} else {
		bitxor2:
			m_Tokens.push_back(Token(TokenType::BitXor, "^", m_Line, m_Column));
		}
	}
	ISINLINE void Lexer::AddIdentifier() {
		if (!m_IsIdentifier) return;

		m_IsIdentifier = false;
		m_Tokens.push_back(Token(TokenType::Identifer, m_LineSource.substr(m_IdentifierBegin, m_IdentifierEnd - m_IdentifierBegin),
								 m_Line, m_IdentifierBegin));
		if (auto iter = m_Keywords.find(m_Tokens.back().Word()); iter != m_Keywords.end()) {
			m_Tokens.back().Type(iter->second);
		}
	}
}