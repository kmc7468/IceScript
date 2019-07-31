#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace ice {
	enum class MessageType {
		None,
		Note,
		Warning,
		Error,
	};

	class Message final {
	private:
		MessageType m_Type = MessageType::None;
		std::string m_Description;
		std::string m_Location;
		std::string m_Note;

	public:
		Message() noexcept = default;
		Message(MessageType type, std::string description);
		Message(MessageType type, std::string description, std::string location);
		Message(MessageType type, std::string description, std::string location, std::string note);
		Message(const Message& message);
		Message(Message&& message) noexcept;
		~Message() = default;

	public:
		Message& operator=(const Message& message);
		Message& operator=(Message&& message) noexcept;

	public:
		MessageType Type() const noexcept;
		std::string Description() const;
		std::string Location() const;
		std::string Note() const;
	};
}