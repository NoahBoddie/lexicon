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


	std::vector<Operation> RoutineCompiler::CompileBlock(Record& data)
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
			for (auto& entry : data.GetChildren())
			{
				std::vector<Operation> new_stuff = CompileLine(entry, Register::Result);
				cache.insert_range(cache.end(), new_stuff);
			}

		}
		
		return cache;
	}

	


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
				
				if (return_policy && return_policy != IdentityManager::instance->GetInherentType(InherentType::kVoid))
				{
					//This is a hacky way of ensuring how 1 liner stuff works
					
					Conversion out;

					auto convert_result = result.IsConvertToQualified(return_policy, nullptr, &out);

					if (convert_result <= convertFailure)
					{
						report::compile::critical("Expression not convertible to return type.");
					}

					_current = &operations;

					CompUtil::HandleConversion(this, out, result, convert_result);
				}

				break;

			}
				//report::runtime::error("Unsupported syntax type.");
			}
			
			if (operations.empty() == false && operations.back()._instruct != InstructionType::Return) {
				//If there is nothing, needs to emplace a return back
				operations.emplace_back(InstructionType::Return);
			}


			PopTargetObject();
			
			operations.shrink_to_fit();
		
			//Dead header scope should validate the returns, making sure that it actually has one.
		}

		RoutineBase data{ operations, varCount[1], argCount[1] };

		return data;
	}



	//I'll be including the source in the other place later, but rn, I'm too fucking lazy.
}