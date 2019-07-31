#include <ice/Encoding.hpp>

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
		switch (firstByte)
		{
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
}