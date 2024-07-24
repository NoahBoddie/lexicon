#include "Lexicon/Engine/CompileUtility.h"
#include "Lexicon/Engine/RoutineCompiler.h"
namespace LEX
{
	bool CompUtil::HandleConversion(ExpressionCompiler* compiler, Conversion& out, Solution& value, ConvertResult convert_result)
	{

		if (out) {
			//If out exists, this means there's something that can be used to convert it. 
			// however, this does NOT work when this conversion needs to be baked into the function.

			//Now granted, because this is in real time, I can just make an instruction handle this.
			// But that instruction would have to do it over and over and over again.

			bool fall = false;

			switch (convert_result)
			{
			case ConvertResult::ImplDefined:
				compiler->GetOperationList().emplace_back(InstructionType::Convert, compiler->GetPrefered(), Operand{ out.implDefined, OperandType::Callable }, value);
				break;

			case ConvertResult::UserDefined:
			resume:

				compiler->GetOperationList().emplace_back(InstructionType::Convert, compiler->GetPrefered(), Operand{ out.userDefined, OperandType::Function }, value);


				if (!fall)
					break;

				[[fallthrough]];

			case ConvertResult::UserToImplDefined:
				if (!fall) {
					fall = true;
					goto resume;
				}

				compiler->GetOperationList().emplace_back(
					InstructionType::Convert,
					compiler->GetPrefered(),
					Operand{ out.userToImpl, OperandType::Callable },
					Operand{ compiler->GetPrefered(), OperandType::Register });

				break;

			default:
				return false;
			}


			//This shouldn't really be using the previous policy, but I kinda don't care for now.
			//TODO: This should be using CompUtil::Mutate
			value = Solution{ value.policy, OperandType::Register, compiler->GetPrefered() };

			return true;
		}

		return false;
	}
}