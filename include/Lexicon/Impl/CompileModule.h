#pragma once

#include "Lexicon/Impl/Operand.h"

namespace LEX::Impl
{
	struct CompilerModule : public ProcessContext
	{
		//This entire thing is used for both operators AND expressions. Works on roughly everything
		//Compiler goes too because it's part of the process. Also for croaking.
		virtual Operand Compile(Record& ast, Script* script, Compiler* compiler) = 0;
	};
}