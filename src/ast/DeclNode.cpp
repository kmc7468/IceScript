#include <ice/ast/DeclNode.hpp>

namespace ice::ast {
	VariableDeclNode::VariableDeclNode(Token startToken, std::string name, TypeNode* type) noexcept
		: StatementNode(std::move(startToken)), Name(std::move(name)), Type(type), Initialization(nullptr) {
	}
	VariableDeclNode::VariableDeclNode(Token startToken, std::string name, ExpressionNode* initialization) noexcept
		: StatementNode(std::move(startToken)), Name(std::move(name)), Type(nullptr), Initialization(initialization) {
	}
	VariableDeclNode::VariableDeclNode(Token startToken, std::string name, TypeNode* type, ExpressionNode* initialization) noexcept
		: StatementNode(std::move(startToken)), Name(std::move(name)), Type(type), Initialization(initialization) {
	}
	VariableDeclNode::~VariableDeclNode() {
		delete Type;
		delete Initialization;
	}

	std::string VariableDeclNode::ToString(std::size_t depth) const {
		const std::string indent = GetIndent(depth);

		std::string result = indent + "VariableDeclNode(\n";

		++depth;
		result += indent + "Name: \"" + Name + "\"\n";
		result += indent + "Type:" + (Type == nullptr ? " null" : '\n' + Type->ToString(++depth) + '\n');

		return result;
	}
}