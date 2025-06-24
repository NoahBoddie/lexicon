#include "Lexicon/Engine/Instruction.h"

#include "Lexicon/Engine/Runtime.h"
#include "Lexicon/Engine/Target.h"
#include "Lexicon/Engine/Operand.h"
#include "Lexicon/Engine/OperandType.h"
#include "Lexicon/Engine/Operation.h"
#include "Lexicon/Engine/InstructionType.h"

namespace LEX
{
	void Instruction::Execute(Runtime* runtime)
	{
		//GetTarget- use this instead.

		//*
		RuntimeVariable result;
		
		try
		{
			report::runtime::trace("Starting: {}; Operands: L: {} R: {}", magic_enum::enum_name(_instruct), magic_enum::enum_name(_ltype), magic_enum::enum_name(_rtype));
			//It's possible that this should possibly return.
			instructList[_instruct].Operate(result, runtime, Operand{ _lhs, _ltype }, Operand{ _rhs, _rtype }, _instruct);
		}
		catch (nullptr_t)
		{
			//Caught an issue.
			//What this should actually be catching are minor errors and such, things that would need to trace. Then, this will send out it's own error, that
			// includes the record data indicated. The code should be able to query the callable unit for this.
		}

		//If not void and complete (or just complete because void isn't a complete type
		//If it's void, that's fine
		if (_out != Register::Invalid && result.IsEmpty() == false)
			//runtime->GetRegister(_out) = std::move(result);
			//TODO:While move would be preferable here, it crashes here right now. Address this at some point. I think maybe cause it doesn't unhandle other?
			runtime->GetRegister(_out) = result;
		//*/
	}

}