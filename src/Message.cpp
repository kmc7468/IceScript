#include <ice/Message.hpp>

#include <iostream>
#include <sstream>
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

	std::string Message::ToString() const {
		std::ostringstream oss;
		
		if (!m_Location.empty()) {
			oss << m_Location << ": ";
		}

		switch (m_Type) {
		case MessageType::Note:
			oss << "note: ";
			break;

		case MessageType::Warning:
			oss << "warning: ";
			break;

		case MessageType::Error:
			oss << "error: ";
			break;
		}

		oss << m_Description;

		if (!m_Note.empty()) {
			oss << '\n' << m_Note;
		}

		return oss.str();
	}
}

namespace ice {
	Messages::Messages(const Messages& messages)
		: m_Messages(messages.m_Messages) {
	}
	Messages::Messages(Messages&& messages) noexcept
		: m_Messages(std::move(messages.m_Messages)) {
	}

	Messages& Messages::operator=(const Messages& messages) {
		m_Messages = messages.m_Messages;

		return *this;
	}
	Messages& Messages::operator=(Messages&& messages) noexcept {
		m_Messages = std::move(messages.m_Messages);

		return *this;
	}

	void Messages::Clear() noexcept {
		m_Messages.clear();
	}
	bool Messages::IsEmpty() const noexcept {
		return m_Messages.empty();
	}
	bool Messages::HasErrors() const noexcept {
		for (const Message& message : m_Messages) {
			if (message.Type() == MessageType::Error) return true;
		}
		return false;
	}
	void Messages::Print() const {
		bool isFirst = true;
		for (const Message& message : m_Messages) {
			if (isFirst) {
				isFirst = false;
			} else {
				std::cout << '\n';
			}

			std::cout << message.ToString() << "\n";
		}
	}

	void Messages::Add(Message message) {
		m_Messages.push_back(std::move(message));
	}
}