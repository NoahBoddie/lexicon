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


namespace LEX
{


	std::vector<Operation> RoutineCompiler::CompileBlock(SyntaxRecord& data)
	{
		//NEW NAME: ProcessNest
		//Scratch, Process/CompileBlock

		std::vector<Operation> cache;

		//Just need a new scope so scope dies faster than cache.
		{
			//This needs a process scope, but that scope will need something for stuff like ifs and elses.
			// Rather, variables created within the if statements line should count.

			//Needs a 

			//Scope a_scope { this, ScopeType::Required };
			std::unique_ptr<Scope> a_scope;

			//This just prevents it from doing this on whatever, but it'd be better to just do it manually.
			if (GetScope() == nullptr)
				a_scope = std::make_unique<Scope>(this, ScopeType::Required);


			//In that case, scopes are objects that are manually created, and require at least a RoutineCompiler (meaning expressions can't create new scopes)
			//, allowing for something that can be created in one function, kept alive until it's end, and then destroyed once no longer needed. Thus, my worries of maintaining
			// the var stack is finished.
			for (auto& entry : data.children())
			{
				std::vector<Operation> new_stuff = CompileLine(entry, Register::Result);
				cache.insert_range(cache.end(), new_stuff);
			}

		}
		
		return cache;
	}

	

#ifdef LEGACY_CompileRoutine
	RoutineBase RoutineCompiler::CompileRoutine()
	{
		//SO I would very much like to removed the excess use of scope here. The FunctionData is here for a reason, 
		// pull the parameter info please. It's a local too, so it works damn it.


		//This automatically searching for the code is probably not the best idea. Maybe change the response depending on what's encountered?
		std::vector<Operation> operations;

		//Exists only to declare scope so I can deal with the lack of a return before the end of the function.
		{
			_current = &operations;

			Scope a_scope{ this, ScopeType::Header };

			/*
			std::vector<ParameterInfo> params = _targetFunc->GetParameters();
			
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
			
			
			varCount[0] = varCount[1] = GetParamAllocSize();
			

			auto end = operations.end();

			switch (funcRecord.SYNTAX().type)
			{
			case SyntaxType::Function: {
				//operations = CompileBlock(*funcRecord.FindChild("code"));
				operations.insert_range(end, CompileBlock(*funcRecord.FindChild(parse_strings::code)));
				break;
			}

			default: {//case SyntaxType::Return:
				//operations = CompileLine(funcRecord, Register::Result);
				_current = &operations;
				
				
				implicitReturn = true;
				//operations.insert_range(end, CompileLine(funcRecord, Register::Result, result));
				Solution result = PushExpression(funcRecord, Register::Result);
				


				QualifiedType return_policy = GetReturnType();
				

				auto void_type = IdentityManager::instance->GetInherentType(InherentType::kVoid);
				if (return_policy && return_policy != void_type)
				{
					//This is a hacky way of ensuring how 1 liner stuff works
					
					auto voidable = IdentityManager::instance->GetInherentType(InherentType::kVoidable);

					//This will ensure that a given return will return a real loaded Variable with void as it's type.
					if (voidable == return_policy && result == void_type){
						operations.emplace_back(InstructionType::DefineVariable, result, Operand{ void_type, OperandType::Type });
					}
					else//Convert should only take place if the result type isn't void.
					{
						Conversion out;

						auto convert_result = result.IsConvertToQualified(return_policy, nullptr, &out);

						if (convert_result <= convertFailure)
						{
							report::compile::critical("Expression not convertible to return type.");
						}

						_current = &operations;

						CompUtil::HandleConversion(this, out, result, convert_result);
					}
				}

				break;

			}
				//report::runtime::error("Unsupported syntax type.");
			}
			
			if (!operations.empty() && operations.back()._instruct != InstructionType::Return) {
				//If there is nothing, needs to emplace a return back
				operations.emplace_back(InstructionType::Return);
			}


			PopTargetObject();
			
			operations.shrink_to_fit();
		
			//Dead header scope should validate the returns, making sure that it actually has one.
		}

		RoutineBase data{ std::move(operations), varCount[1], argCount[1] };

		return data;
	}
#endif

	RoutineBase RoutineCompiler::CompileRoutine()
	{
		//SO I would very much like to removed the excess use of scope here. The FunctionData is here for a reason, 
		// pull the parameter info please. It's a local too, so it works damn it.


		//This automatically searching for the code is probably not the best idea. Maybe change the response depending on what's encountered?
		std::vector<Operation> operations;

		//Exists only to declare scope so I can deal with the lack of a return before the end of the function.
		{
			_current = &operations;

			Scope a_scope{ this, ScopeType::Header };

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


			varCount[0] = varCount[1] = GetParamAllocSize();


			auto end = operations.end();

			QualifiedType return_policy = GetReturnType();

			auto void_type = IdentityManager::instance->GetInherentType(InherentType::kVoid);
			auto voidable = IdentityManager::instance->GetInherentType(InherentType::kVoidable);
			
			Solution result;

			switch (funcRecord.SYNTAX().type)
			{
			case SyntaxType::Function: {
				//operations = CompileBlock(*funcRecord.FindChild("code"));
				operations.insert_range(end, CompileBlock(*funcRecord.FindChild(parse_strings::code)));

				//~~~~~~~~~~~~~~~~~~~~~~~~~~
				//This is messy as shit I know.
				//if (voidable == return_policy && !operations.empty() && operations.back()._instruct != InstructionType::Return) {
				//	operations.emplace_back(InstructionType::DefineVariable, Operand{ Register::Result, OperandType::Register }, Operand{ void_type, OperandType::Type });
				//}
				break;
			}

			default: {//case SyntaxType::Return:
				//operations = CompileLine(funcRecord, Register::Result);
				_current = &operations;


				implicitReturn = true;
				//operations.insert_range(end, CompileLine(funcRecord, Register::Result, result));
				result = PushExpression(funcRecord, Register::Result);



				QualifiedType return_policy = GetReturnType();



				if (return_policy && return_policy != void_type)
				{
					//This is a hacky way of ensuring how 1 liner stuff works



					//TODO:The check below needs to happen regardless of whatever type is at hand.

					//This will ensure that a given return will return a real loaded Variable with void as it's type.
					//if (voidable == return_policy && result == void_type) {
					//	operations.emplace_back(InstructionType::DefineVariable, result, Operand{ void_type, OperandType::Type });
					//}
					//else//Convert should only take place if the result type isn't void.
					if (voidable != return_policy && result != void_type)
					{
						Conversion out;

						auto convert_result = result.IsConvertToQualified(return_policy, nullptr, &out);

						if (convert_result <= convertFailure)
						{
							report::compile::critical("Expression not convertible to return type.");
						}

						_current = &operations;

						CompUtil::HandleConversion(this, out, result, convert_result);
					}
				}

				break;

			}
				   //report::runtime::error("Unsupported syntax type.");
			}

			//if (!operations.empty() && operations.back()._instruct != InstructionType::Return) {
			if (!operations.empty() && a_scope.IsReturned() == false) {
				
				
				_current = &operations;//Doing this is stupid and I hate it but fuck it whatever.
				CompUtil::PrepareReturn(this, return_policy, result);
				operations.emplace_back(InstructionType::Return);
			}


			PopTargetObject();

			operations.shrink_to_fit();

			//Dead header scope should validate the returns, making sure that it actually has one.
		}

		RoutineBase data{ std::move(operations), varCount[1], argCount[1] };

		return data;
	}



	//I'll be including the source in the other place later, but rn, I'm too fucking lazy.
}