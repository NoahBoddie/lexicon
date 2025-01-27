#include "Lexicon/Engine/CompileUtility.h"
#include "Lexicon/Engine/RoutineCompiler.h"
#include "Lexicon/Impl/common_type.h"
namespace LEX
{
	bool CompUtil::HandleConversion(ExpressionCompiler* compiler, Conversion& out, Solution& value, ConvertResult convert_result, Register reg)
	{

		if (out) {
			//If out exists, this means there's something that can be used to convert it. 
			// however, this does NOT work when this conversion needs to be baked into the function.

			//Now granted, because this is in real time, I can just make an instruction handle this.
			// But that instruction would have to do it over and over and over again.

			bool fall = false;

			
			//Before using prefered, it should check if the given solution has a register it'd like to use. This makes the most sense to be honest.
			if (reg == Register::Invalid) {
				//if (value.Get(reg) == false)
					reg = compiler->GetPrefered();

			}

			switch (convert_result)
			{
			case ConvertResult::ImplDefined:
				compiler->GetOperationList().emplace_back(InstructionType::Convert, reg, Operand{ out.implDefined, OperandType::Callable }, value);
				break;

			case ConvertResult::UserDefined:
			resume:

				compiler->GetOperationList().emplace_back(InstructionType::Convert, reg, Operand{ out.userDefined, OperandType::Function }, value);


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
					reg,
					Operand{ out.userToImpl, OperandType::Callable },
					Operand{ reg, OperandType::Register });

				break;

			default:
				return false;
			}


			//This shouldn't really be using the previous policy, but I kinda don't care for now.
			//TODO: This should be using CompUtil::Mutate
			value = Solution{ value.policy, OperandType::Register, reg };

			return true;
		}

		return false;
	}

	void CompUtil::PrepareReturn(ExpressionCompiler* compiler, QualifiedType return_type, Solution value)
	{
		auto a_void = common_type::void_t();
		if ((!value || a_void == value) && a_void != return_type) {
			//Solution has no value and it is not void
			
			//Actually, if it's void you'll want to clear it even more
			
			compiler->GetOperationList().emplace_back(InstructionType::DefineVariable, Operand{ Register::Result, OperandType::Register }, Operand{ return_type.policy, OperandType::Type });
		}
	}
}