#pragma once

#include "RoutineCompiler.h"

namespace LEX
{
	struct ITypePolicy;


	enum struct ScopeType : uint8_t
	{
		Required,		//It's a given that the parent scope will have to run the following when encountered
		Conditional,	//The parent scope may not run the contents of this scope when encountered
		Depedent,		//Whether the parent scope runs the contents of this scope are dependent on another scope.
	};

	struct Scope
	{
	private:
		enum ReturnType
		{
			None,		//There is no guarenteed return
			Nested,		//There is a return, through different scopes.
			Immutable	//There is a non conditional return interpreted. Cannot be overwritten.
		};
	public:

		//The generic object that handles the concept of a scope.

		~Scope()
		{
			//When the scope of the function using scope decays, it will set its parent scope back to being the main scope, as well as
			// adding a decrement value to the variable count.


			//There's just one problem. You see, it's that pesky fucking loose operation stack you see.  No way to add to it from destruction.

			//Okay, what if I do a best of both worlds. The return for operations are within the compiler, and are submitted to a buffer? Doesn't work for similar reasons.

			//Here's what I can do. There's a wrapper to the compile function calls, similar to try module and such




			_CheckExit();


			process->currentScope = parent;

			process->ModVarCount(-variables.size());

			//Note, still needs to inject the decrement.

			process->currentScope = parent;


			//If stuff like out is to be used, I'll need
		}


		[[nodiscard]] Scope(RoutineCompiler* compiler, ScopeType s) :process{ static_cast<RoutineCompiler*>(compiler) }, _type{ s }
		{
			//When created, it will set itself as the main scope, and record the old scope.
			parent = process->currentScope;
			process->currentScope = this;
		}


		size_t CreateVariable(std::string name, ITypePolicy*)
		{
			auto result = variables.size();
			variables.push_back(name);
			process->ModVarCount(1);

			return result;
		}

		size_t GetVariable(std::string name)
		{
			//Would like a different way to handle this, maybe with records instead, so I can inspect if it does the "SearchGlobals" syntax.
			//My head hurts, so just imagine this shit actually returned.

			return 0;
		}


		std::vector<Field*> SearchField(std::string name, ITypePolicy*)
		{
			return {};
			//This basically needs to search through itself first for results before it starts searching environments and such.
		}

		//If this is the scope that the routine uses to hold its top level data returns true.
		bool IsRoutine() const { return !parent; }

		//As variables get declared, this list will grow in size.
		// Variable might need to be an object. If it is, I think mixing it with compiled operand maybe useful. Whatever this is, it would need qualifiers.
		// qualifiers that don't really exist at a later point, and just direct the compiler on how to handle things.
		std::vector<std::string> variables{};

		std::map<std::string, VariableInfo> vars;

		//For use with stuff like in, out, and ref
		std::vector<decltype(vars)::iterator> assignments;


		//If scope has no parent, it will address the function data stored on the compiler, making it the chief scope.
		Scope* parent = nullptr;

		RoutineCompiler* process = nullptr;//Will turn statement compiler into a routinecompiler.


		const ScopeType _type;



		ReturnType _return;

		void _CheckExit()
		{
			if (IsRoutine() == true) {
				return;
			}

			switch (_type)
			{
			case ScopeType::Required:
				if (_return)
					parent->_ConfirmExit();

				break;

			case ScopeType::Conditional:
				if (_return)
					parent->_FlagExit();
				else
					parent->_UnflagExit();

				break;

			case ScopeType::Depedent:
				if (!_return)
					parent->_UnflagExit();

				break;
			}
		}

		void _FlagExit()
		{

			if (_return != Immutable)
				_return = ReturnType::Nested;
		}

		void _UnflagExit()
		{
			if (_return != Immutable)
				_return = ReturnType::None;
		}

		void _ConfirmExit()
		{
			_return = ReturnType::Immutable;
		}

		//Flag for a return call within this scope. But it means different things for the routine's scope than it does for something like an
		// if scope. So I gotta work that out.



		//Make sure to remove copy assignment stuff. Doesn't need it. Shouldn't leave its initial function

	};

}