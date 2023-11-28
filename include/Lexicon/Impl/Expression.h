#pragma once

#include "Lexicon/Impl/TypeAliases.h"
#include "Lexicon/Impl/ExpressionType.h"

namespace LEX::Impl
{
	struct Syntax
	{
		SyntaxType type{ SyntaxType::Total };
		Column column{ 0 };
		Line line{ 0 };


		std::string Print()
		{
			if (type == SyntaxType::Total)
				return "Syntax: Header";
			else
				return std::format("Syntax: {} (col: {}/ line: {}", ExpressionToString(type), column, line);
		}

		operator std::string() { return Print(); }


		Syntax() = default;
		Syntax(SyntaxType t) : type{ t } {}
	};
	static_assert(sizeof(Syntax) == 0x8);

#define SYNTAX() GetEnum<Syntax>()

}