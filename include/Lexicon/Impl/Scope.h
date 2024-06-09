#pragma once

#include "RoutineCompiler.h"
#include "SearchPredicate.h"

//*src?
#include "VariableInfo.h"//InfoBase
#include "QualifiedField.h"

namespace LEX
{
	struct ITypePolicy;
	struct PolicyBase;
	struct FunctionInfo;


	enum struct ScopeType : uint8_t
	{
		Required,		//It's a given that the parent scope will have to run the following when encountered
		Conditional,	//The parent scope may not run the contents of this scope when encountered
		Depedent,		//Whether the parent scope runs the contents of this scope are dependent on another scope.
		Header,			//The header scope, the only things that go here would be parameters and such. Cannot have a parent.


		//Header is the default scope,
		//Required is any scope that will be encountered (say {..})
		//Condition is stuff like ifs and while loops.
		//Dependent means it's reliant on a conditional to trigger. 
		// I'm considering something like trivial, say if something's set within an if statement.
	};


	struct Scope
	{
	private:
		enum ReturnType : uint8_t
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

			//If current exception is happening fuck this shit, the entire thing is ending.
			if (IsTopLevel() && std::current_exception() != nullptr)
				return;


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
				report::compile::fatal("invalid header scope with parent detected.");
			}
		}



		LocalInfo* CreateVariable(std::string name, QualifiedType type)
		{
			//Should consider not using a pointer due to invalidation. Instead, maybe give a copy.

			assert(type.policy);

			bool header = IsHeader();

			//auto result = variables.size();

			//variables.push_back(name);

			if (auto& var = vars[name]; var) {
				report::compile::critical("Variable name '{}' already taken", name);
			}

			auto index = !header ? process->ModVarCount(type.policy) : process->ModParamCount(type.policy);


			RGL_LOG(debug, "Attempting to create {} at index {}", name, index);

			LocalInfo& result = vars[name] = LocalInfo{ type, (uint32_t)index };


			//This is only temporarily valid.
			return &result;
		}

		//LocalInfo* GetLocalVariable(std::string& name)
		//VariableInfo* GetGlobalVariable(std::string& name);




		PolicyBase* SearchTypePath(Record& _path);

		FunctionInfo* SearchFunctionPath(Record& path, OverloadKey& input, Overload& out);

		QualifiedField SearchFieldPath(Record& _path, OverloadKey* key = nullptr);


		//If this is the scope that the routine uses to hold its top level data returns true.
		bool IsRoutine() const { return !parent; }

		bool IsHeader() const { return _type == ScopeType::Header && !parent; }

		bool IsTopLevel() const { return IsHeader() || (parent ? parent->_type == ScopeType::Header : false); }


		//As variables get declared, this list will grow in size.
		// Variable might need to be an object. If it is, I think mixing it with compiled operand maybe useful. Whatever this is, it would need qualifiers.
		// qualifiers that don't really exist at a later point, and just direct the compiler on how to handle things.
		//std::vector<std::string> variables{};

		std::map<std::string, LocalInfo> vars;

		//For use with stuff like in, out, and ref
		std::vector<Field*> assignments;


		//If scope has no parent, it will address the function data stored on the compiler, making it the chief scope.
		Scope* parent = nullptr;

		RoutineCompiler* process = nullptr;//Will turn statement compiler into a routinecompiler.


		const ScopeType _type;



		ReturnType _return{};

		std::string name();

		void _CheckExit()
		{

			if (IsHeader() == true) {

				if (!_return && process->GetReturnType()->FetchTypeID() != -1)
					report::compile::fatal("Explicit return expected. {}", name());

				return;
			}

			switch (_type)
			{
			case ScopeType::Header:
				break;

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


		void FlagReturn()
		{
			_return = ReturnType::Immutable;
		}

		//Flag for a return call within this scope. But it means different things for the routine's scope than it does for something like an
		// if scope. So I gotta work that out.



		//Make sure to remove copy assignment stuff. Doesn't need it. Shouldn't leave its initial function

	};

	namespace Hide
	{
		/*
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
					report::compile::fatal("invalid header scope with parent detected.");
				}
			}


			std::vector<size_t> CreateVariables(std::vector<std::string> names, std::vector<ITypePolicy*> policies)
			{
				//Make this the primary version if you would.
				bool header = IsHeader();


				size_t size = policies.size();

				if (names.size() != size) {
					report::compile::critical("Amount of names do not match the amount of policies.");
				}

				std::vector<size_t> result{};

				result.resize(size);

				for (int i = 0; i < size; i++)
				{
					std::string& name = names[i];
					ITypePolicy* policy = policies[i];

					if (auto& var = vars[name]; var) {
						report::compile::critical("Variable name already taken");
					}

					auto index = !header ? process->ModVarCount(policy) : process->ModParamCount(policy);

					RGL_LOG(debug, "Scope creating {} at index {}, policy {}", name, index, !!policy);

					vars[name] = VariableInfo{ policy, index, Qualifier::None };

					result[i] = index;
				}

				return result;
			}


			VariableInfo* CreateVariable(std::string name, QualifiedType type)
			{
				//Should consider not using a pointer due to invalidation. Instead, maybe give a copy.

				assert(type.policy);

				bool header = IsHeader();

				//auto result = variables.size();

				//variables.push_back(name);

				if (auto& var = vars[name]; var) {
					report::compile::critical("Variable name already taken");
				}

				auto index = !header ? process->ModVarCount(type.policy) : process->ModParamCount(type.policy);


				RGL_LOG(debug, "Attempting to create {} at index {}", name, index);

				VariableInfo& result = vars[name] = VariableInfo{ type.policy, index, type.flags };


				//This is only temporarily valid.
				return &result;
			}

			VariableInfo* GetVariable(std::string& name)
			{
				//Would like a different way to handle this, maybe with records instead, so I can inspect if it does the "SearchGlobals" syntax.
				//My head hurts, so just imagine this shit actually returned.

				//This should be using the below.

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

			//TODO: make a SearchType function in scope, TypeAliases being here would be peachy.

			Field* SearchField(std::string name, OverloadKey& key, FieldPredicate pred = nullptr);

			//I will abet this for now, but it dies some day
			Field* SearchField(std::string name, FieldPredicate pred = nullptr);


			ITypePolicy* SearchType(std::string name);

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

			std::string name();

			void _CheckExit()
			{

				if (IsHeader() == true) {

					if (!_return && process->GetReturnType()->FetchTypeID() != -1)
						report::compile::fatal("Explicit return expected. {}", name());

					return;
				}

				switch (_type)
				{
				case ScopeType::Header:
					break;

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


			void FlagReturn()
			{
				_return = ReturnType::Immutable;
			}

			//Flag for a return call within this scope. But it means different things for the routine's scope than it does for something like an
			// if scope. So I gotta work that out.



			//Make sure to remove copy assignment stuff. Doesn't need it. Shouldn't leave its initial function

		};
		//*/
	}
}