#pragma once

#include <ice/ast/Node.hpp>

#include <string>

namespace ice::ast {
	struct VariableDeclNode final : StatementNode {
		const std::string Name;
		TypeNode* const Type;
		ExpressionNode* const Initialization;

		VariableDeclNode(Token startToken, std::string name, TypeNode* type) noexcept;
		VariableDeclNode(Token startToken, std::string name, ExpressionNode* initialization) noexcept;
		VariableDeclNode(Token startToken, std::string name, TypeNode* type, ExpressionNode* initialization) noexcept;
		virtual ~VariableDeclNode() override;

		virtual std::string ToString(std::size_t depth) const override;
	};
}