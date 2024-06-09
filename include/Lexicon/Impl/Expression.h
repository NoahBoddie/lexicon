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
		Syntax(SyntaxType t, Column c = 0, Line l = 0) : 
			type{ t },  
			column{ c },
			line{ l }
		{}
	};
	static_assert(sizeof(Syntax) == 0x8);

//Short hand to get syntax from an AST. Made so it can only be used on records to prevent mistakes.
#define SYNTAX() GetEnumFromRecord<LEX::Impl::Syntax>()

}