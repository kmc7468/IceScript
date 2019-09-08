#pragma once

#include <ice/Lexer.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace ice::ast {
	struct Node {
		const Token StartToken;

		Node(Token startToken) noexcept;
		virtual ~Node() = default;

		virtual std::string ToString(std::size_t depth) const = 0;

	protected:
		static std::string GetIndent(std::size_t depth);
	};

	struct StatementNode : Node {
		using Node::Node;
	};

	struct ExpressionNode : StatementNode {
		using StatementNode::StatementNode;
	};

	struct BlockNode final : StatementNode {
		const std::vector<StatementNode*> Statements;

		BlockNode(Token startToken, std::vector<StatementNode*> statements) noexcept;
		virtual ~BlockNode() override;

		virtual std::string ToString(std::size_t depth) const override;
	};

	struct TypeNode : Node {
		using Node::Node;
	};

	struct IdentifierNode final : virtual TypeNode, virtual ExpressionNode {
		const std::string Name;

		IdentifierNode(Token startToken, std::string name) noexcept;

		virtual std::string ToString(std::size_t depth) const override;
	};
}