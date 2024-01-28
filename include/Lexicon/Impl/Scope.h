#pragma once

#include "RoutineCompiler.h"

//*src?
#include "VariableInfo.h"//InfoBase

namespace LEX
{
	struct ITypePolicy;


	enum struct ScopeType : uint8_t
	{
		Required,		//It's a given that the parent scope will have to run the following when encountered
		Conditional,	//The parent scope may not run the contents of this scope when encountered
		Depedent,		//Whether the parent scope runs the contents of this scope are dependent on another scope.
		Header,			//The header scope, the only things that go here would be parameters and such. Cannot have a parent.
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

			
			//if (!parent)//if it's top level, that instead decides if it even removes anything.
			if (IsTopLevel() == false)
				process->ModVarCount(-vars.size());
			
			process->currentScope = parent;



			//If stuff like out is to be used, I'll need


			//TODO: If I can, I'd like some optimization that collapses all variable increments
			// ^ perhaps such optimizations can be made here in post, with the ability to insert instructions at a certain position.
			// all I would need is the index of where this scope started, then once this ends I'd submit all the instructions
			// for creating them en masse, and each instruction to load the type policy onto the variable
		}


		[[nodiscard]] Scope(RoutineCompiler* compiler, ScopeType s) :process{ static_cast<RoutineCompiler*>(compiler) }, _type{ s }
		{
			//When created, it will set itself as the main scope, and record the old scope.

			//This needs to handle the return type so it can get a proper compiling error for not returning when required.

			parent = process->currentScope;
			process->currentScope = this;

			if (parent && s == ScopeType::Header) {
				RGL_LOG(critical, "invalid header scope with parent detected.");
			}
		}


		std::vector<size_t> CreateVariables(std::vector<std::string> names, std::vector<ITypePolicy*> policies)
		{
			//Make this the primary version if you would.
			bool header = IsHeader();

			
			size_t size = policies.size();
			
			if (names.size() != size) {
				RGL_LOG(critical, "Amount of names do not match the amount of policies.");
				throw nullptr;
			}

			std::vector<size_t> result{};
			
			result.resize(size);

			for (int i = 0; i < size; i++)
			{
				std::string& name = names[i];
				ITypePolicy* policy = policies[i];
				
				if (auto& var = vars[name]; var)
				{
					RGL_LOG(critical, "Variable name already taken");
					throw nullptr;
				}

				auto index = !header ? process->ModVarCount(policy) : process->ModParamCount();

				RGL_LOG(debug, "Attempting to create {} at index {}", name, index);

				vars[name] = VariableInfo{ policy, index };

				result[i] = index;
			}

			return result;
		}

		size_t CreateVariable(std::string name, ITypePolicy* policy)
		{
			bool header = IsHeader();

			//auto result = variables.size();
			
			//variables.push_back(name);

			if (auto& var = vars[name]; var)
			{
				RGL_LOG(critical, "Variable name already taken");
				throw nullptr;
			}

			auto index = !header ? process->ModVarCount(policy) : process->ModParamCount();

			
			RGL_LOG(debug, "Attempting to create {} at index {}", name, index);

			vars[name] = VariableInfo{ policy, index };
			

			return index;
		}

		VariableInfo* GetVariable(std::string& name)
		{
			//Would like a different way to handle this, maybe with records instead, so I can inspect if it does the "SearchGlobals" syntax.
			//My head hurts, so just imagine this shit actually returned.
			
			auto end = vars.end();
			
			if (auto it = vars.find(name); it != end) {
				return &it->second;
			}
			else if (parent) {
				return parent->GetVariable(name);
			}
			else {
				return nullptr;
			}
		}


		std::vector<Field*> SearchField(std::string name, ITypePolicy* = nullptr)
		{
			auto var = GetVariable(name);

			if (var)
				return { var };
			else
				return {};

			//This basically needs to search through itself first for results before it starts searching environments and such.
		}

		//If this is the scope that the routine uses to hold its top level data returns true.
		bool IsRoutine() const { return !parent; }

		bool IsHeader() const { return _type == ScopeType::Header && !parent; }

		bool IsTopLevel() const { return IsHeader() || (parent ? parent->_type == ScopeType::Header : false); }


		//As variables get declared, this list will grow in size.
		// Variable might need to be an object. If it is, I think mixing it with compiled operand maybe useful. Whatever this is, it would need qualifiers.
		// qualifiers that don't really exist at a later point, and just direct the compiler on how to handle things.
		//std::vector<std::string> variables{};

		std::map<std::string, VariableInfo> vars;

		//For use with stuff like in, out, and ref
		//std::vector<decltype(vars)::iterator> assignments;


		//If scope has no parent, it will address the function data stored on the compiler, making it the chief scope.
		Scope* parent = nullptr;

		RoutineCompiler* process = nullptr;//Will turn statement compiler into a routinecompiler.


		const ScopeType _type;



		ReturnType _return{};

		void _CheckExit()
		{
			if (IsHeader() == true) {
				//TODO: If it's top level, it needs to check it's return status.
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