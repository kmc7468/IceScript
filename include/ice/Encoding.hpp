#pragma once

namespace ice {
	int GetCodepointLength(char firstByte) noexcept;
	bool IsDigit(char firstByte) noexcept;
	bool IsWhitespace(char firstByte) noexcept;
	bool IsEOL(char firstByte) noexcept;
}