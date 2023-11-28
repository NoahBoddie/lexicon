#pragma once

#include "Operation.h"

#include "Runtime.h"
#include "Target.h"
#include "Operand.h"
#include "OperandType.h"
#include "Instruction.h"
#include "InstructionType.h"

namespace LEX
{
	void Operation::Execute(Runtime* runtime)
	{
		//GetTarget- use this instead.

		//*
		RuntimeVariable result;

		try
		{
			RGL_LOG(trace, "start {}", _instruct);
			instructList[_instruct].Operate(result, runtime, Operand{ _lhs, _ltype }, Operand{ _rhs, _rtype });//It's possible that this should possibly return.
			RGL_LOG(trace, "end");
		}
		catch (nullptr_t)
		{
			//Caught an issue.
			//What this should actually be catching are minor errors and such, things that would need to trace. Then, this will send out it's own error, that
			// includes the record data indicated. The code should be able to query the callable unit for this.
		}

		//If not void and complete (or just complete because void isn't a complete type
		//If it's void, that's fine
		if (_out != Impl::Register::Invalid && result.IsVoid() == false)
			runtime->GetRegister(_out) = result;
		//*/
	}

}