#pragma once

#include "Lexicon/Impl/TypeAliases.h"
#include "Lexicon/Impl/ExpressionType.h"

namespace LEX::Impl
{
	struct Expression
	{
		ExpressionType type{ ExpressionType::Total };
		Column column{ 0 };
		Line line{ 0 };


		std::string Print()
		{
			if (type == ExpressionType::Total)
				return "Expression: Header";
			else
				return std::format("Expression: {} (col: {}/ line: {}", ExpressionToString(type), column, line);
		}

		operator std::string() { return Print(); }


		Expression() = default;
		Expression(ExpressionType t) : type{ t } {}
	};
	static_assert(sizeof(Expression) == 0x8);

#define EXPRESSION() GetEnum<Expression>()

}