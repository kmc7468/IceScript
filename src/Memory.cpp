#include <ice/Memory.hpp>

namespace ice {
	Stack::Stack()
		: Stack(1 * 1024 * 1024) {
	}
	Stack::Stack(std::size_t size)
		: m_Data(new std::uint8_t[size]), m_Size(size) {
	}
	Stack::Stack(Stack&& stack) noexcept
		: m_Data(stack.m_Data), m_Size(stack.m_Size) {
		stack.m_Data = nullptr;
		stack.m_Size = 0;
	}
	Stack::~Stack() {
		delete m_Data;
	}

	Stack& Stack::operator=(Stack&& stack) noexcept {
		delete m_Data;

		m_Data = stack.m_Data;
		m_Size = stack.m_Size;
	
		stack.m_Data = nullptr;
		stack.m_Size = 0;

		return *this;
	}
	const std::uint8_t& Stack::operator[](std::size_t index) const noexcept {
		return m_Data[index];
	}
	std::uint8_t& Stack::operator[](std::size_t index) noexcept {
		return m_Data[index];
	}

	std::size_t Stack::Size() const noexcept {
		return m_Size;
	}
}