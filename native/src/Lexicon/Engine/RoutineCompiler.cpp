#pragma once

#include "Lexicon/Engine/RoutineCompiler.h"

#include "Lexicon/Engine/Expression.h"
#include "Lexicon/Engine/ExpressionType.h"
#include "Lexicon/Engine/Solution.h"
#include "Lexicon/Engine/Operation.h"
#include "Lexicon/Engine/RoutineBase.h"

//*src
#include "Lexicon/Engine/Scope.h"
#include "Lexicon/Engine/FunctionData.h"
#include "Lexicon/Engine/ParameterInfo.h"

#include "Lexicon/Engine/parse_strings.h"

#include "Lexicon/Impl/common_type.h"
namespace LEX
{
	std::vector<Operation>* CompilerBase::GetOperationListPtr()
	{
		if (_current)
			return _current;

		if (auto scope = GetScope())
			return &scope->GetOperationList();

		return nullptr;
	}


	std::vector<Operation>& CompilerBase::GetOperationList()
	{
		return *GetOperationListPtr();
	}



	void RoutineCompiler::CompileBlock(SyntaxRecord& data)
	{		
		//This just prevents it from doing this on whatever, but it'd be better to just do it manually.
		if (GetScope() == nullptr)
			report::fault::critical("CompileBlock requires a scope to run.");


		//In that case, scopes are objects that are manually created, and require at least a RoutineCompiler (meaning expressions can't create new scopes)
		//, allowing for something that can be created in one function, kept alive until it's end, and then destroyed once no longer needed. Thus, my worries of maintaining
		// the var stack is finished.
		for (auto& entry : data.children())
		{
			CompileLine(entry, Register::Result);
		}

	}


	void RoutineCompiler::CompileBlock(SyntaxRecord& data, ScopeType type)
	{
		Scope scope{ this, type };

		CompileBlock(data);
	}

	




	bool RoutineCompiler::CompileRoutine(RoutineBase& routine)
	{
		//SO I would very much like to removed the excess use of scope here. The FunctionData is here for a reason, 
		// pull the parameter info please. It's a local too, so it works damn it.



		//Exists only to declare scope so I can deal with the lack of a return before the end of the function.
		
		try
		{
			std::vector<Operation> operations;

			{
				//<KILL> _current = &operations;

				Scope a_scope{ this, ScopeType::Header, &operations };

				/*
				//Kinda feel like this should be used but it isn't. This could cause some variables to go without value.
				std::vector<ParameterInfo> params = _callData->GetParameters();

				std::vector<ITypePolicy*> types;

				std::transform(params.begin(), params.end(), std::back_inserter(types), [&](ParameterInfo& it) {return it.GetType(); });

				ModParamCount(types.size(), types);


				//*/

				bool method = false;

				Solution solution{ GetTargetType(), OperandType::Index, 0 };


				//Assign const here.

				TargetObject target{ solution };

				if (solution.policy) {
					PushTargetObject(target);
					method = true;
				}

				//TODO: Here's where parameter allocation actually happens seemingly. While I don't want to change that, this needs to define params.
				varCount[0] = varCount[1] = GetParamAllocSize();


				auto end = operations.end();

				QualifiedType return_policy = GetReturnType();
				
				//auto void_type = IdentityManager::instance->GetInherentType(InherentType::kVoid);
				//auto voidable = IdentityManager::instance->GetInherentType(InherentType::kVoidable);

				auto void_type = common_type::void_t();
				auto voidable = common_type::voidable();

				Solution result;

				switch (funcRecord.GetSyntax().type)
				{
				case SyntaxType::Function: {
					//operations = CompileBlock(*funcRecord.FindChild("code"));
					CompileBlock(*funcRecord.FindChild(parse_strings::code));

					//~~~~~~~~~~~~~~~~~~~~~~~~~~
					//This is messy as shit I know.
					//if (voidable == return_policy && !operations.empty() && operations.back()._instruct != InstructionType::Return) {
					//	operations.emplace_back(InstructionType::DefineVariable, Operand{ Register::Result, OperandType::Register }, Operand{ void_type, OperandType::Type });
					//}
					break;
				}

				default: {//case SyntaxType::Return:
					//operations = CompileLine(funcRecord, Register::Result);
					//<KILL> _current = &operations;


					implicitReturn = true;
					//operations.insert_range(end, CompileLine(funcRecord, Register::Result, result));
					result = PushExpression(funcRecord, Register::Result);



					QualifiedType return_policy = GetReturnType();



					if (return_policy && return_policy != void_type)
					{
						//This is a hacky way of ensuring how 1 liner stuff works



						//The check below needs to happen regardless of whatever type is at hand.

						//This will ensure that a given return will return a real loaded Variable with void as it's type.
						//if (voidable == return_policy && result == void_type) {
						//	operations.emplace_back(InstructionType::DefineVariable, result, Operand{ void_type, OperandType::Type });
						//}
						//else//Convert should only take place if the result type isn't void.
						if (voidable != return_policy && result != void_type)
						{
							//<KILL> _current = &operations;

							Conversion out;

							auto convert_result = result.IsConvertToQualified(return_policy, nullptr, &out);

							if (convert_result <= convertFailure)
							{
								report::compile::error("Expression not convertible to return type.");
							}

							CompUtil::HandleConversion(this, out, result, return_policy, convert_result);
						}
					}

					break;

				}
					   //report::runtime::error("Unsupported syntax type.");
				}



				//if (!operations.empty() && operations.back()._instruct != InstructionType::Return) {
				if (!a_scope.IsEmpty() && a_scope.IsReturned() == false) {
					//I know this is important, but why is this happening here

					//<KILL> _current = &operations;//Doing this is stupid and I hate it but fuck it whatever.
					CompUtil::PrepareReturn(this, return_policy, result);
					GetOperationList().emplace_back(InstructionType::Return);
				}


				PopTargetObject();

				if (a_scope.IsHeaderSatisfied() == false)
					report::compile::error("Explicit return expected. {}", name());

				//Dead header scope should validate the returns, making sure that it actually has one.
			}

			operations.shrink_to_fit();

			routine = RoutineBase{ std::move(operations), varCount[1], argCount[1] };
			return _success;
		}
		catch (CompileError& error)
		{
			//Now, it can actually submit a message, but I'll not care right now.
			return false;
		}
	}



	//I'll be including the source in the other place later, but rn, I'm too fucking lazy.














	size_t CompilerBase::ModVarCount(int64_t inc)
	{
		//Here, we use what the scope has, except if it's not present, we'll make it ourselves.


		size_t count = varCount[0];

		if ((varCount[0] += inc) > varCount[1])
			varCount[1] = varCount[0];


		if (inc > 0 && currentScope)
		{
			//Handling this through the scope allows a single entry to be incremented, instead of dealing with each increment one instruct at a time
			currentScope->IncrementVarCount(inc);
		}
		else {

			auto& op_list = GetOperationList();

			op_list.emplace_back(InstructionType::IncrementVarStack, Operand{ inc , OperandType::Differ });

		}

		return count;
	}

}