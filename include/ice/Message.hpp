#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

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

	public:
		std::string ToString() const;
	};

	std::string CreateMessageLocation(std::size_t line, std::size_t column);
	std::string CreateMessageLocation(const std::string& source);
	std::string CreateMessageLocation(const std::string& source, std::size_t line, std::size_t column);
	std::string CreateMessageNoteLocation(const std::string& source, std::size_t line, std::size_t column, std::size_t length);

	class Messages final {
	private:
		std::vector<Message> m_Messages;

	public:
		Messages() noexcept = default;
		Messages(const Messages& messages);
		Messages(Messages&& messages) noexcept;
		~Messages() = default;

	public:
		Messages& operator=(const Messages& messages);
		Messages& operator=(Messages&& messages) noexcept;

	public:
		void Clear() noexcept;
		bool IsEmpty() const noexcept;
		bool HasErrors() const noexcept;
		void Print() const;

		void Add(Message message);
		void AddNote(const std::string& description, const std::string& source);
		void AddNote(const std::string& description, const std::string& source, std::size_t line, std::size_t column);
		void AddNote(const std::string& description, const std::string& source, std::size_t line, std::size_t column, const std::string& note);
		void AddWarning(const std::string& description, const std::string& source, std::size_t line, std::size_t column);
		void AddWarning(const std::string& description, const std::string& source, std::size_t line, std::size_t column, const std::string& note);
		void AddError(const std::string& description, const std::string& source, std::size_t line, std::size_t column);
		void AddError(const std::string& description, const std::string& source, std::size_t line, std::size_t column, const std::string& note);
	};
}