#pragma once

namespace ice {
	int GetCodepointLength(char firstByte) noexcept;
	bool IsDigit(char firstByte) noexcept;
	bool IsWhitespace(char firstByte) noexcept;
	bool IsEOL(char firstByte) noexcept;

	bool IsFullWidth(char32_t character) noexcept;
	bool IsHalfWidth(char32_t character) noexcept;
}