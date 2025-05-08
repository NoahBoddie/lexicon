#pragma once

#include "Lexicon/Engine/RoutineCompiler.h"

//*src?
#include "Lexicon/Engine/VariableInfo.h"//InfoBase
#include "Lexicon/Engine/QualifiedField.h"

namespace LEX
{
	struct ITypeInfo;
	struct TypeBase;
	struct FunctionInfo;



	struct Scope
	{
	public:
		enum ReturnType : uint8_t
		{
			None,			//There is no guarenteed return
			ImpNested,		//There is a return, through different scopes.
			Nested,			//There is a return, through different scopes.
			Immutable		//There is a non conditional return interpreted. Cannot be overwritten.
		};
		//The generic object that handles the concept of a scope.

		~Scope()
		{
			Release();
		}

		//Seriously, this version should only be given to certain functions.
		std::vector<Operation>&& Release(std::function<void(std::vector<Operation>*)> func)
		{
			//This should not be used by many things, so probably friend functioning this some how. I don't want this to be used
			// if it's through GetScope 
			
			//This is the true destructor.
			if (!_finished) {
				_finished = true;


				if (process)
				{
					_CheckExit();

					//I may likely end it here if it's not satisied

					//if (!parent)//if it's top level, that instead decides if it even removes anything.
					if (MustDecrement() == true)
						process->ModVarCount(-(int64_t)vars.size());



					if (func) {
						//If func has a request, it does something before it finishes the scope.
						func(_out);
					}

					if (_out) {


						_out->insert_range(_out->end(), operationList);
					}

					//<KILL>process->_current = _prev;

					process->currentScope = _parent;
					
				}
			}

			return std::move(operationList);


			//If stuff like out is to be used, I'll need


			//TODO: If I can, I'd like some optimization that collapses all variable increments
			// ^ perhaps such optimizations can be made here in post, with the ability to insert instructions at a certain position.
			// all I would need is the index of where this scope started, then once this ends I'd submit all the instructions
			// for creating them en masse, and each instruction to load the type policy onto the variable
		}


		std::vector<Operation>&& Release()
		{
			return std::move(Release(nullptr));
		}

		std::vector<Operation>& GetOperationList();

		//So the out is where all the information goes, and the previous is the previous operation list.

		[[nodiscard]] Scope(RoutineCompiler* compiler, ScopeType s, std::vector<Operation>* out = nullptr) :
			process{ compiler }, 
			_type{ s },
			_out { out ? out : process->GetOperationListPtr() }
		{
			//TODO: Replace this with a proper check, or make scopes derive from a temporary variable differently than one that doesn't.
			assert(!process->IsDetached());

			
			//When created, it will set itself as the main scope, and record the old scope.

			//This needs to handle the return type so it can get a proper compiling error for not returning when required.

			_parent = process->currentScope;
			process->currentScope = this;
			
			//<KILL>process->_current = &operationList;

			if (_parent && s == ScopeType::Header) {
				report::fault::critical("invalid header scope with parent detected.");
			}
		} 
		
		[[nodiscard]] Scope(RoutineCompiler* compiler, ScopeType s, std::vector<Operation>& out) :Scope{ compiler, s, &out }
		{
			
		}





		void IncrementVarCount(int64_t inc = 1)
		{
			if (inc < 1) {
				//TODO: warn and move on
				return;
			}

			//I'd like to go through this if incrementing through something like inlining
			auto& alloc = ObtainAllocator();
			alloc._lhs.differ += inc;

			if (IsConstant() == false){
				//This should prevent the incredibly large allocations. Constant scopes won't disrupt anything,
				// but conditional and dependent scopes may not happen, leading to decrements, and it being unaccounted in the grand scheme of it.
				// This is the best of both worlds. Notably, only happens on an increment.
				ClearAllocParent();
			}
		}


		LocalInfo* CreateVariable(std::string name, QualifiedType type)
		{
			//Should consider not using a pointer due to invalidation. Instead, maybe give a copy.

			assert(type.policy);
			
			//TODO: Replace this with a proper check
			assert(!process->IsDetached());

			bool header = IsHeader();

			//auto result = variables.size();

			//variables.push_back(name);

			if (auto& var = vars[name]; var) {
				report::compile::critical("Variable name '{}' already taken", name);
			}

			//auto index = !header ? process->ModVarCount(type.policy) : process->ModParamCount(type.policy);
			auto index = process->InitLocal(type.policy);


			RGL_LOG(debug, "Attempting to create {} at index {}", name, index);

			LocalInfo& result = vars[name] = LocalInfo{ type, (uint32_t)index };



			//This is only temporarily valid.
			return &result;
		}

		//LocalInfo* GetLocalVariable(std::string& name)
		//VariableInfo* GetGlobalVariable(std::string& name);

		bool MustDecrement() const
		{
			return IsTopLevel() == false && vars.size() != 0;
		}

		


		ITypeInfo* SearchTypePath(SyntaxRecord& _path);

		FunctionNode SearchFunctionPath(SyntaxRecord& path, OverloadArgument& input, Overload& out);

		QualifiedField SearchFieldPath(SyntaxRecord& _path, OverloadArgument* key = nullptr);


		Scope* parent() { return _parent; }

		Scope* grandparent()
		{
			Scope* focus = _parent;

			if (focus)
			{
				
				while (focus->parent() != nullptr)
				{
					focus = focus->parent();
				}
			}
			
			return focus;
		}

		//If this is the scope that the routine uses to hold its top level data returns true.
		bool IsRoutine() const { return !_parent; }

		bool IsHeader() const { return _type == ScopeType::Header && !_parent; }

		bool IsTopLevel() const { return IsHeader() || (_parent ? _parent->_type == ScopeType::Header : false); }

		bool IsEmpty() const { return operationList.empty(); }

		bool IsConstant() const { return _type == ScopeType::Required || _type == ScopeType::Header; }

		//As variables get declared, this list will grow in size.
		// Variable might need to be an object. If it is, I think mixing it with compiled operand maybe useful. Whatever this is, it would need qualifiers.
		// qualifiers that don't really exist at a later point, and just direct the compiler on how to handle things.
		//std::vector<std::string> variables{};

		std::map<std::string, LocalInfo> vars;

		//For use with stuff like in, out, and ref
		std::vector<Field*> assignments;


		//If scope has no parent, it will address the function data stored on the compiler, making it the chief scope.
		Scope* _parent = nullptr;

		RoutineCompiler* process = nullptr;//Will turn statement compiler into a routinecompiler.




		std::vector<Operation>* _out = nullptr;
		

		std::vector<Operation> operationList;
		size_t allocator = -1;

		const ScopeType _type;
		bool _finished = false;
		bool _detached = false;

		ReturnType _return{};

		bool IsHeaderSatisfied() const
		{
			return process->implicitReturn || IsReturned() || process->GetReturnType()->TryRuleset(TypeRuleset::ReturnOpt);
		}

		void _CheckExit()
		{

			if (IsHeader() == true) {
				return;
			}

			switch (_type)
			{
			case ScopeType::Header:
				break;

			case ScopeType::Required:
				if (_return)
					_parent->_ConfirmExit();

				break;

			case ScopeType::Conditional:
				if (IsReturned() == true)
					//Will be nested if dependent was successful. And if it wasn't, an imp exit is discared.
					_parent->IsReturnNested() ? _parent->_ConfirmExit() : _parent->_FlagImpExit();
				else
					_parent->_UnflagExit();

				break;

			case ScopeType::Depedent:
				if (IsReturned() == true)
					_parent->IsReturnImpNested() ? _parent->_ConfirmExit() : _parent->_FlagExit();
				else
					_parent->_UnflagExit();

				//else if (IsReturnImmutable() == true && _parent->IsReturnI)
				break;
			}
		}
		
		void _FlagExit()
		{

			if (_return != Immutable)
				_return = ReturnType::Nested;
		}


		void _FlagImpExit()
		{

			if (_return != Immutable)
				_return = ReturnType::ImpNested;
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
			

		bool IsReturnNested() const
		{
			return _return == ReturnType::Nested;
		}

		bool IsReturnNone() const
		{
			return _return == ReturnType::ImpNested || _return == ReturnType::None;
		}

		bool IsReturnImpNested() const
		{
			return _return == ReturnType::ImpNested;
		} 

		bool IsReturned() const
		{
			return _return == ReturnType::Immutable;
		}

		//Flag for a return call within this scope. But it means different things for the routine's scope than it does for something like an
		// if scope. So I gotta work that out.



		//Make sure to remove copy assignment stuff. Doesn't need it. Shouldn't leave its initial function
	protected:

		Operation& ObtainAllocator()
		{
			auto& list = operationList;
			 
			//What this allocates to seems to be the problem
			if (allocator == -1){
				allocator = list.size();
				return list.emplace_back(InstructionType::ModVarStack, Operand{ 0 , OperandType::Differ });
			}
			else{
				return list[allocator];
			}
		}


		void ClearAllocParent()
		{
			if (_parent)
				_parent->ClearAlloc();
		}

		void ClearAlloc()
		{
			//Thi
			allocator = -1; 

			ClearAllocParent();
		}


	};
}