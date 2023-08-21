#pragma once

#include "Lexicon/Impl/CompileModule.h"

namespace LEX::Impl
{

	//This is basically the idea.
	Operand CompileExpression(Record& ast, Script* script, ExpressionType type)
	{
		return {};
	}


	Script* temp_GetScript(std::string name)
	{
		return nullptr;
	}

	Script* temp_CreateScript(std::string name)
	{
		return nullptr;
	}

	struct Project_;
	struct Script_;

	struct CompileModule;

	//I might template this
	struct CompileHandler
	{
	public:
		static void AddModule(CompilerModule* mdl);



		static GENERIC_ARRAY(CompilerModule*) GetModuleList();

		//Not sure if I actually want this
		static CompilerModule* GetModuleByName(std::string_view);

	private:
		static void _CheckSort();


		//Not sure if I really want one list for this, maybe when requesting the depth I'll pull out only ones of a certain depth
		inline static GENERIC_ARRAY(CompilerModule*) _moduleList;
		inline static bool _sorted = true;

	};

	//should be paired with a string
	enum struct CompileResult
	{
		Success,
		Tentive,
		//Failures
	};

	struct Compiler;

	using CompileFunc = void(Record&, Compiler*);

	//inline std::map<ExpressionType, CompileFunc*> TEMP(compileMap);

	struct Compiler
	{
		//Memory would be nice here. The memory will probably be records. I can treat the numbers as
		// numbers, and the strings as names/strings, and have them able to have hierarchy and such.
		// only trival data goes here though, stuff that I'll be fine with being erased once everything dies
		// which it will and should, because the compiler will start and stop a lot.







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



		//May be the only thing I need.
		Environment* _environment = nullptr;

		void SetEnvironment(Environment* env)
		{
			_environment = env;
		}


		//The parser here should walk through very similarly to how the other streams do (though, this isn't a stream you walk through really).


		void croak(std::string msg) {
			//This croak needs to come with a few other things, like what record is actually being used.
			// There's a situation where I can have an "outer croak" which bails to CompileExpression, then compile expression outputs what record it is,
			// and just have an "inner" one that takes a specific non header record to be more specific.
			throw CompilingError("temp");
		}



	private:

		//This is basically the idea.
		void _CompileExpression(Record& ast)
		{
			//Temp compile map is quelled for right now.
			/*
			auto it = temp_compileMap.find(ast.EXPRESSION().type);
			
			if (temp_compileMap.end() != it) {
				it->second(ast, this);
			}
			else {
				throw CompilingError("No function for that expression type");
			}
			//*/
		}

		void _CompileStatement(Record& statement)
		{
			//Compiling a statement looks rougly something like this. Can probably reduce this down a bit.

			//TODO: Also worth noting, at this point this data should be const, and so should the access of it's data. We'll want no more children,


			//preprocessors aren't really preprocessor and are taken in the order they're viewed in. They additionally dont
			// leave the scope they're introduced in, unless included.

			get_switch(statement.EXPRESSION().type)
			{
			case ExpressionType::Script:
				throw CompilingError("cant be script");

			case ExpressionType::Header:
				throw CompilingError("literally non-functional wtf");

			case ExpressionType::Project:
			//case ExpressionType::Directive:
				//These types are skipped over, not really used here.
				return;

			}

			_CompileExpression(statement);
		}


	public:



		//The idea of these 2 core ideas is that Compile script is looping against all the children, while compile just compiles it to the one environment.
		// It would be like submitting a function or a property to the thing.
		//They'll also need to be static and catch failure.

		//Later, this will need just need the script, not both this and the environment.
		void CompileScript(Record& top, Script* script)
		{
			if (top.GetEnum<Expression>().type != ExpressionType::Script)
				throw CompilingError("not script expression");

			//Confirm script and record are indeed in association.


			SetEnvironment(script);
		

			for (auto& child : top.GetChildren()) {
				_CompileStatement(child);
			}
		}

		//So there's the concept where it has to be compiled as a script, and one where it just compiles it to an environment.
		// Compile to environment means the script already exists

		void CompileComponent(Record& top, Environment* env)
		{

			SetEnvironment(env);

			get_switch(top.EXPRESSION().type)
			{
			case ExpressionType::Project:
				throw CompilingError("cant be project");
			}


			_CompileStatement(top);
		}

		
	};



	struct Compiler__
	{
		//void Compile()
	};
}