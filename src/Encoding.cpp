#include <ice/Encoding.hpp>

#include <algorithm>
#include <map>

namespace {
	const std::map<std::pair<char32_t, char32_t>, char> s_EastAsianWidthTable = {
#include "detail/EastAsianWidthTable.txt"
	};
}

namespace ice {
	int GetCodepointLength(char firstByte) noexcept {
		unsigned char firstByteUnsigned = static_cast<unsigned char>(firstByte);

		if (firstByteUnsigned < 0x80) {
			return 1;
		} else if ((firstByteUnsigned & 0xF0) == 0xF0) {
			return 4;
		} else if ((firstByteUnsigned & 0xE0) == 0xE0) {
			return 3;
		} else {
			return 2;
		}
	}
	bool IsDigit(char firstByte) noexcept {
		return '0' <= firstByte && firstByte <= '9';
	}
	bool IsWhitespace(char firstByte) noexcept {
		switch (firstByte) {
		case ' ':
		case '\t':
			return true;

		default:
			return false;
		}
	}
	bool IsEOL(char firstByte) noexcept {
		switch (firstByte) {
		case '\r':
		case '\n':
			return true;

		default:
			return false;
		}
	}

	bool IsFullWidth(char32_t character) noexcept {
		if (character < 0x80) return false;
		for (auto iter = s_EastAsianWidthTable.begin(); iter != s_EastAsianWidthTable.end(); ++iter) {
			if (iter->first.first <= character && character <= iter->first.second) return iter->second == 'F';
		}
		return false;
	}
	bool IsHalfWidth(char32_t character) noexcept {
		if (character < 0x80) return true;
		for (auto iter = s_EastAsianWidthTable.begin(); iter != s_EastAsianWidthTable.end(); ++iter) {
			if (iter->first.first <= character && character <= iter->first.second) return iter->second == 'H';
		}
		return true;
	}

	char32_t GetCodepoint(const char* begin, int length) noexcept {
		switch (length) {
		case 1:
			return *begin;
			
		case 2:
			return ((static_cast<unsigned char>(*begin) & 0x1F) << 6) +
				   (static_cast<unsigned char>(*(begin + 1)) & 0x3F);

		case 3:
			return ((static_cast<unsigned char>(*begin) & 0x0F) << 12) +
				   ((static_cast<unsigned char>(*(begin + 1)) & 0x3F) << 6) +
				   (static_cast<unsigned char>(*(begin + 2)) & 0x3F);

		case 4:
			return ((static_cast<unsigned char>(*begin) & 0x07) << 18) +
				   ((static_cast<unsigned char>(*(begin + 1)) & 0x3F) << 12) +
				   ((static_cast<unsigned char>(*(begin + 2)) & 0x3F) << 6) +
				   (static_cast<unsigned char>(*(begin + 3)) & 0x3F);

		default:
			return 0;
		}
	}
}