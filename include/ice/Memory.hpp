#pragma once

#include <cstddef>
#include <cstdint>

namespace ice {
	class Stack final {
	private:
		std::uint8_t* m_Data = nullptr;
		std::size_t m_Size = 0;

	public:
		Stack();
		Stack(std::size_t size);
		Stack(Stack&& stack) noexcept;
		~Stack();
		
	public:
		Stack& operator=(Stack&& stack) noexcept;
		const std::uint8_t& operator[](std::size_t index) const noexcept;
		std::uint8_t& operator[](std::size_t index) noexcept;

	public:
		std::size_t Size() const noexcept;
	};
}