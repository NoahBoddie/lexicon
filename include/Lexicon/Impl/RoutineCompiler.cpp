#pragma once

#include "RoutineCompiler.h"

#include "Expression.h"
#include "ExpressionType.h"
#include "Solution.h"
#include "Operation.h"
#include "RoutineBase.h"

//*src
#include "Scope.h"
#include "FunctionData.h"
#include "ParameterInfo.h"

#include "parse_strings.h"


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
			Scope a_scope{ this, ScopeType::Required };//Is this supposed to be the one doing this?

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

	std::vector<Operation> RoutineCompiler::CompileHeader()
	{
		//NEW NAME: ProcessNest
		//Scratch, Process/CompileBlock

		std::vector<Operation> result;


		Scope a_scope{ this, ScopeType::Header };

		auto old = _current;
		_current = &result;

		std::vector<ParameterInfo> params = _targetFunc->GetParameters();

		size_t size = params.size();

		std::vector<std::string> names{ size };
		std::vector<ITypePolicy*> policies{ size };

		//names.reserve(size);
		//policies.reserve(size);

		for (int i = 0; i < size; i++)
		{
			auto& param = params[i];

			names[i] = param.GetFieldName();

			policies[i] = param.GetType();
		}
		
		//a_scope.CreateVariables(names, policies);


		_current = old;

		return result;
	}


	RoutineBase RoutineCompiler::CompileRoutine()
	{
		//SO I would very much like to removed the excess use of scope here. The FunctionData is here for a reason, 
		// pull the parameter info please. It's a local too, so it works damn it.


		//This automatically searching for the code is probably not the best idea. Maybe change the response depending on what's encountered?
		std::vector<Operation> operations;

		//Exists only to declare scope so I can deal with the lack of a return before the end of the function.
		{
			Scope a_scope{ this, ScopeType::Header };

			std::vector<ParameterInfo> params = _targetFunc->GetParameters();

			size_t size = params.size();

			bool method = false;

			Solution solution{ _targetFunc->GetTargetType(), OperandType::Index, 0 };
			
			
			//Assign const here.

			TargetObject target{ solution };

			if (solution.policy) {
				PushTargetObject(target);
				method = true;
			}
			
			if (1 != 1)
			{//I think this is the stuff I can cut out.
				std::vector<std::string> names{ size + method };
				std::vector<ITypePolicy*> policies{ size + method };

				if (method) {
					names[0] = "<this>";
					policies[0] = solution.policy;
				}

				for (int i = 0; i < size; i++) {
					auto& param = params[i];
					names[i + method] = param.GetFieldName();
					policies[i + method] = param.GetType();
				}

				auto old = _current;
				_current = &operations;


				//logger::debug("Parameter Size: {}", size + method);
				//a_scope.CreateVariables(names, policies);


				_current = old;

			}
			else
			{
				varCount[0] = varCount[1] = _targetFunc->GetParamAllocSize();
			}

			auto end = operations.end();

			switch (funcRecord.SYNTAX().type)
			{
			case SyntaxType::Function: {
				//operations = CompileBlock(*funcRecord.FindChild("code"));
				operations.insert_range(end, CompileBlock(*funcRecord.FindChild(parse_strings::code)));
				break;
			}

			case SyntaxType::Return:
				//operations = CompileLine(funcRecord, Register::Result);
				operations.insert_range(end, CompileLine(funcRecord, Register::Result));
				break;

			default:
				report::runtime::error("Unsupported syntax type.");
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