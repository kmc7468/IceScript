#include <ice/ast/Node.hpp>

#include <utility>

namespace ice::ast {
	Node::Node(Token startToken) noexcept
		: StartToken(std::move(StartToken)) {
	}

	std::string Node::GetIndent(std::size_t depth) {
		return std::string(depth * 4, ' ');
	}
}

namespace ice::ast {
	BlockNode::BlockNode(Token startToken, std::vector<StatementNode*> statements) noexcept
		: StatementNode(std::move(StartToken)), Statements(std::move(statements)) {
	}
	BlockNode::~BlockNode() {
		for (StatementNode* statement : Statements) {
			delete statement;
		}
	}

	std::string BlockNode::ToString(std::size_t depth) const {
		const std::string indent = GetIndent(depth);

		std::string result = indent + "BlockNode([\n";

		++depth;
		bool isFirst = true;
		for (StatementNode* statement : Statements) {
			if (isFirst) {
				isFirst = false;
			} else {
				result += "\n,";
			}
			result += indent + statement->ToString(depth);
		}

		result += "])";
		return result;
	}
}

namespace ice::ast {
	IdentifierNode::IdentifierNode(Token startToken, std::string name) noexcept
		: TypeNode(startToken), ExpressionNode(startToken), Name(std::move(name)) {
	}

	std::string IdentifierNode::ToString(std::size_t depth) const {
		return GetIndent(depth) + "IdentifierNode(Name: \"" + Name + "\")";
	}
}