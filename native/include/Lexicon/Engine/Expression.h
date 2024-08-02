#pragma once

#include "Lexicon/TypeAliases.h"
#include "Lexicon/Engine/ExpressionType.h"

namespace LEX
{
	struct Syntax
	{
		SyntaxType type{ SyntaxType::Total };
		Column column{ 0 };
		Line line{ 0 };


		std::string Print() const
		{
			if (type == SyntaxType::Total)
				return "Syntax: Header";
			else
				return std::format("Syntax: {} (col: {}/ line: {})", magic_enum::enum_name(type), column, line);
		}

		std::string location() const
		{
			return std::format("(line: {}, col: {})", line, column);
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
#define SYNTAX() GetEnumFromRecord<LEX::Syntax>()

}