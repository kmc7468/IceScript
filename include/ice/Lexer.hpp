#pragma once

#include <cstddef>
#include <string>

namespace ice {
	enum class TokenType {
		None,
		Whitespace,
		EOL,

		Identifer,
		BinInteger,
		OctInteger,
		DecInteger,
		HexInteger,
		Decimal,
		Character,
		String,

		Plus, // +
		Increment, // ++
		PlusAssign, // +=
		Minus, // -
		Decrement, // --
		MinusAssign, // -=
		Multiply, // *
		MultiplyAssign, // *=
		Divide, // /
		DivideAssign, // /=
		Modulo, // %
		ModuloAssign, // %=
		Exponent, // **
		ExponentAssign, // **=

		Assign, // =
		Equal, // ==
		NotEqual, // !=
		Greater, // >
		GreaterEqaul, // >=
		Less, // <
		LessEqual, // <=
		And, // &&
		Or, // ||
		Not, // !

		BitAnd, // &
		BitAndAssign, // &=
		BitOr, // |
		BitOrAssign, // |=
		BitXor, // ^
		BitXorAssign, // ^=
		BitNot, // ~
		BitLeftShift, // <<
		BitLeftShiftAssign, // <<=
		BitRightShift, // >>
		BitRightShiftAssign, // >>=

		RightwardsArrow, // ->
		RightwardsDoubleArrow, // =>

		LeftBrace, // {
		RightBrace, // }
		LeftParen, // (
		RightParen, // )
		LeftBigParen, // [
		RightBigParen, // ]

		Dot, // .
		Comma, // ,
		Semicolon, // ;
		Colon, // :
		Question, // ?

		ModuleKeyword, // module
		ImportKeyword, // import

		Int8Keyword, // int8
		Int16Keyword, // int16
		Int32Keyword, // int32
		Int64Keyword, // int64
		Int128Keyword, // int128
		IntPtrKeyword, // intptr
		UInt8Keyword, // uint8
		UInt16Keyword, // uint16
		UInt32Keyword, // uint32
		UInt64Keyword, // uint64
		UInt128Keyword, // uint128
		UIntPtrKeyword, // uintptr
		Float32Keyword, // float32
		Float64Keyword, // float64
		NumberKeyword, // number
		VoidKeyword, // void
		BoolKeyword, // bool
		TrueKeyword, // true
		FalseKeyword, // false
		CharKeyword, // char
		Char8Keyword, // char8
		StringKeyword, // string
		String8Keyword, // string8
		NullKeyword, // null
		AnyKeyword, // any

		EnumKeyword, // enum
		ClassKeyword, // class
		ThisKeyword, // this
		SuperKeyword, // super
		VirtualKeyword, // virtual
		OverrideKeyword, // override
		SealedKeyword, // sealed
		PubKeyword, // pub
		PrivKeyword, // priv
		TraitKeyword, // trait
		IsKeyword, // is
		AsKeyword, // as

		FunctionKeyword, // function
		ReturnKeyword, // return

		IfKeyword, // if
		ElseKeyword, // else
		SwitchKeyword, // switch
		CaseKeyword, // case
		ForKeyword, // for
		WhileKeyword, // while
		DoKeyword, // do
		BreakKeyword, // break
		ContinueKeyword, // continue

		VarKeyword, // var
		LetKeyword, // let
		MutKeyword, // mut
		StaticKeyword, // static
		NewKeyword, // New

		ThrowKeyword, // throw
		TryKeyword, // try
		CatchKeyword, // catch

		SizeOfKeyword, // sizeof
		TypeOfKeyword, // typeof
		NameOfKeyword, // nameof
	};

	class Token final {
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
	};
}