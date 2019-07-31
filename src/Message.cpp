#include <ice/Message.hpp>

#include <utility>

namespace ice {
	Message::Message(MessageType type, std::string description)
		: m_Type(type), m_Description(std::move(description)) {
	}
	Message::Message(MessageType type, std::string description, std::string location)
		: m_Type(type), m_Description(std::move(description)), m_Location(std::move(location)) {
	}
	Message::Message(MessageType type, std::string description, std::string location, std::string note)
		: m_Type(type), m_Description(std::move(description)), m_Location(std::move(location)), m_Note(std::move(note)) {
	}
	Message::Message(const Message& message)
		: m_Type(message.m_Type), m_Description(message.m_Description), m_Location(message.m_Location), m_Note(message.m_Note) {
	}
	Message::Message(Message&& message) noexcept
		: m_Type(message.m_Type), m_Description(std::move(message.m_Description)), m_Location(std::move(message.m_Location)), m_Note(std::move(message.m_Note)) {
		message.m_Type = MessageType::None;
	}

	Message& Message::operator=(const Message& message) {
		m_Type = message.m_Type;
		m_Description = message.m_Description;
		m_Location = message.m_Location;
		m_Note = message.m_Note;

		return *this;
	}
	Message& Message::operator=(Message&& message) noexcept {
		m_Type = std::move(message.m_Type);
		m_Description = std::move(message.m_Description);
		m_Location = std::move(message.m_Location);
		m_Note = std::move(message.m_Note);

		return *this;
	}

	MessageType Message::Type() const noexcept {
		return m_Type;
	}
	std::string Message::Description() const {
		return m_Description;
	}
	std::string Message::Location() const {
		return m_Location;
	}
	std::string Message::Note() const {
		return m_Note;
	}
}