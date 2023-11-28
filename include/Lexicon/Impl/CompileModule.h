#pragma once

#include "Lexicon/Impl/Operand.h"

namespace LEX::Impl
{
	struct Compiler;

	//The name is compile module btw, not CompilerModule
	struct CompilerModule : public ProcessContext
	{
		//Unlike compile modules, very few and far between will be singleton statics that can be used to process them.
		// things like the script compiler

		//This entire thing is used for both operators AND expressions. Works on roughly everything
		//Compiler goes too because it's part of the process. Also for croaking.
		virtual Operand Compile(Record& ast, Script* script, Compiler* compiler) = 0;

		//V2, instead of script, I'll be using environment. Script will not be the only type that this
		// can belong to, as the commons its a special type of file. That, and it can belong to classes.
		virtual Operand Compile(Compiler* compiler, Record& ast, Environment* env) = 0;


		//bool CanHandle, (const Record& ast)
		//{
		//	return false;
		//}
		 
		 
		//regardless of which version is used, it's important to note that the ast by this point is a thing that
		// exists elsewhere persistently.
	};
}