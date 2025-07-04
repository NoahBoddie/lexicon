#include "Lexicon/Engine/CompileUtility.h"
#include "Lexicon/Engine/RoutineCompiler.h"
#include "Lexicon/Engine/Scope.h"
#include "Lexicon/Impl/common_type.h"
namespace LEX
{
	bool CompUtil::HandleConversion(ExpressionCompiler* compiler, Conversion& out, Solution& value, const QualifiedType& to,ConvertResult res, SyntaxRecord& target, Register reg)
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

			switch (res.data)
			{
			case ConversionEnum::ImplDefined:
				compiler->EmplaceInstruction(target, InstructionType::Convert, reg, Operand{ out.implDefined, OperandType::Callable }, value);
				break;

			case ConversionEnum::UserDefined:
			resume:

				compiler->EmplaceInstruction(target, InstructionType::Convert, reg, Operand{ out.userDefined, OperandType::Function }, value);


				if (!fall)
					break;

				[[fallthrough]];

			case ConversionEnum::UserToImplDefined:
				if (!fall) {
					fall = true;
					goto resume;
				}

				compiler->EmplaceInstruction(target,
					InstructionType::Convert,
					reg,
					Operand{ out.userToImpl, OperandType::Callable },
					Operand{ reg, OperandType::Register });

				break;

			default:
				return false;
			}


			//This shouldn't really be using the previous policy, but I kinda don't care for now.
			value = Solution{ to, OperandType::Register, reg };
			return true;
		}

		return false;
	}

	bool CompUtil::HandleConversion(ExpressionCompiler* compiler, Solution& from, const QualifiedType& to, SyntaxRecord& target, Register reg, ConversionFlag flags)
	{
		Conversion out;

		auto convert = from.IsConvertToQualified(to, nullptr, &out, flags);
		
		if (!convert) {
			target.error<IssueType::Compile>("Cannot initialize. Error {}", magic_enum::enum_name(convert.data));
		}

		return CompUtil::HandleConversion(compiler, out, from, to, convert, target, reg);

	}


	void CompUtil::PrepareReturn(ExpressionCompiler* compiler, QualifiedType return_type, Solution value)
	{
		auto a_void = common_type::void_t();
		if ((!value || a_void == value) && a_void != return_type) {
			//Solution has no value and it is not void
			
			//Actually, if it's void you'll want to clear it even more
			//TODO: I wish to know why this doesn't properly log the location of the return.
			compiler->EmplaceInstruction(InstructionType::DefineVariable, Operand{ Register::Result, OperandType::Register }, Operand{ return_type.policy, OperandType::Type });
		}
	}

	int64_t CompUtil::SkipScope(RoutineCompiler* compiler, SyntaxRecord& target, const Operand& condition, bool negate, uint64_t offset)
	{
		auto scope = compiler->GetScope();
		auto& list = compiler->GetInstructionList();

		if (scope->IsHeader() == true) {
			report::fault::critical("Header scope cannot be skipped (Also, I need a record pls).");
		}

		int64_t size = (int64_t)list.size();

		scope->Release([&](std::vector<Instruction>* out)
			{
				auto size = (int64_t)list.size();

				//if (size +  offset) {
					//We'll only place these if there's actually somethin

					auto instruct = negate ? InstructType::DropStackN : InstructType::DropStack;
					
					//From an optimal standpoint, I'd like to place this first, and have everything else done after.
					
					compiler->InsertInstruction(target, { instruct, Operand{ (int64_t)size + offset + 1, OperandType::Differ }, condition });
					//list.insert(list.begin(), { instruct, Operand{ (int64_t)size + offset + 1, OperandType::Differ }, condition, compiler->UseRecord(target)});
					//out->emplace_back(instruct, Operand{ (int64_t)size , OperandType::Differ }, query);
				//}
			});

		return size;
	
	}
}