#pragma once

#include "Lexicon/Impl/CompileModule.h"

namespace LEX::Impl
{

	//This is basically the idea.
	Operand CompileExpression(Record& ast, Script* script, ExpressionType type)
	{
		return {};
	}

	struct Compiler : public ProcessContext
	{
		std::string_view _context;

		//The idea is that being shown this, I can have the individual parse module be the thing to describe what the parse module is
		// additional thoughts, if I can connect this to a stack allocated object that basically is just a parse module and the previous parse module,
		// kinda like a linked list but shitty and loose, I can have access to all other parsers that have been used.
		//ParseModule* context;

		ProcessChain* contextChain;

		std::string_view GetContext() override
		{
			return _context;
		}
		//Memory would be nice here. The memory will probably be records. I can treat the numbers as
		// numbers, and the strings as names/strings, and have them able to have hierarchy and such.
		// only trival data goes here though, stuff that I'll be fine with being erased once everything dies
		// which it will and should, because the compiler will start and stop a lot.


		//*Ok so I thought I'd need to do order, but seeing as this does everything in parts, I don't need this.
		//constexpr static ExpressionType expressionOrder[]
		//{
		//	ExpressionType::Function
		//	ExpressionType::Function
		//};
		//constexpr static size_t orderSize = sizeof(decltype(expressionOrder));








		//First and foremost, if ExpressionType::Script is detected, it knows it's supposed to go to the environment designated.
		//There's the other problem that I don't compile things all at once and the order they were found in (well, not yet for anything)

		//But records do know who their parents are... So how about for things like functions compiling,


		//Another thing of note, that which returns from a compilation is an operation target, or something that uses an operation target.
		// A lot of fuckery happens with

		//The stages still need to exist. First, declarative compiling, which just adds everything to the environment
		//-after this phase, structs define themselves, which is a linking stage in of itself. 
		// Since environment nor prototype arent vclasses, no greater size (actually, environment might be later.
		//-the the declarative stage happens next, where all functions will

		//So the biggest problem is, should I experience a failure within a value, that isn't runtime based, what should happen? I think
		// the presented object shouldn't be functional

		//Here's what I'll do. There's a parsing section right? So this is the second that actually does something. A lot of the current set up will be 
		// put in there, and it largely works somewhat. 
		// And the idea is if the script section doesn't load properly, it will not create the json/toml parsing.

		//The question is then, how to validate that?
		//I don't actually think I will, I'll just allow it to look up invalid things, and should the thing it look up be valid, the function should levy an error,
		// and fail to function. I'll have a function that prevents one from expecting things to work properly, especially if debug is enabled. 

		//So here's the last thing. Linkage, is what handles the most of this compiling stuff, but once when all forms of linkage have happened, it will do it 
		// immediately.

		Script* source = nullptr;
		//The parser here should walk through very similarly to how the other streams do (though, this isn't a stream you walk through really).


		void croak(std::string msg) {
			//don't care so much about this.
			//throw new ParsingError(msg + " (" + line + ":" + col + ")");
			throw CompilingError("temp");
		}

		void _CompileStatement(Record& statement)
		{
			//Compiling a statement looks rougly something like this. Can probably reduce this down a bit.

			//TODO: Also worth noting, at this point this data should be const, and so should the access of it's data. We'll want no more children,


			//preprocessors aren't really preprocessor and are taken in the order they're viewed in. They additionally dont
			// leave the scope they're introduced in, unless included.

			get_switch(statement.EXPRESSION().type)
			{
			case ExpressionType::Function:
			case ExpressionType::Variable:
			case ExpressionType::Inline:
				CompileExpression(statement, source, switch_value);
				return;
			default:
				throw CompilingError("invalid statement");
			}
		}

		void _SetScriptEvironment(std::string name)
		{

		}

		void _CreateScriptEvironment(std::string name)
		{
			//Should confirm no other script environment exists
			// If so, send error, if not, create and SetScriptEnvironment

			Script* script_environ = nullptr;
			source = script_environ;
			//_SetScriptEnvironment(name);
		}


		void _CompileScript(Record& top)
		{
			if (top.GetEnum<Expression>().type != ExpressionType::Script)
				throw CompilingError("not script expression");

			//Search for script

			_CreateScriptEvironment(top.GetTag());
		}


		void Compile(Record& topLevel)
		{
			//Unsure if I want to copy for this.

			//Compile has a few different modes
			// if the toplevel record is detected, it will attempt to create an evironment, and then will store it.
			// should it try to compile anything else, it will get it's grandparent record, and attempt to get the environment.
			// it not being a script or not having a grandparent record is fine.


			//Check if script.
			// If script script, check for environment. if exists, throw error. if not create.

			//If not script, get grandparent, get environment to search.

			//Then, for both ends, put it through a switch of recognized top level ExpressionTypes, and compile them. If not recognized, throw an error.
			//The very first thing that has to be used is directives, but this should only be if components no longer do linking, because otherwise such a
			// process must be saved for post linking. Just wont worry about that right now.

			//Unsure if I'll actually do this, because that would mean someone is adding new scripts.
			//bool post_link = false;

			if (topLevel.GetEnum<Expression>().type == ExpressionType::Script) {
				_CreateScriptEvironment(topLevel.GetTag());

				auto& children = topLevel.GetChildren();

				for (auto& child : children) {
					_CompileStatement(child);
				}
			}
			else {
				Record* grandpa = topLevel.GetGrandParent();

				//it's allowed to not have a top level script grandparent record, it just won't access anything automatically other than commons
				if (grandpa && grandpa->GetEnum<Expression>().type == ExpressionType::Script) {
					_SetScriptEvironment(topLevel.GetTag());
				}

				_CompileStatement(topLevel);
			}
		}
	};
}