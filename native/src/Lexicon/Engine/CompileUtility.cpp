#include "Lexicon/Engine/CompileUtility.h"
#include "Lexicon/Engine/RoutineCompiler.h"
#include "Lexicon/Engine/Scope.h"
#include "Lexicon/Impl/common_type.h"

#include "Lexicon/Engine/Runtime.h"//Only for an enum
namespace LEX
{


	void CompUtil::LoadParameter(ExpressionCompiler* compiler, SyntaxRecord& record, Solution from, Index index,
		std::optional<bool> is_ref, std::vector<Instruction> express_instructs, std::function<Solution(Solution)> func)
	{
		//Index should actually probably be a differ


		std::vector<Instruction> result;

		//stuff mutate load only uses is ref



		if (from.IsVariadic() == false) {
			result.append_range(express_instructs);//Load the arguments that will have 

			if (func) {
				RoutineCompiler::TempListHandle handle{ result, compiler->_current };
				from = func(from);
			}
			result.append_range(CompUtil::MutateLoad(from, Operand{ index, OperandType::Argument }, is_ref));
		}
		else
		{


			//This should be able to able handle non-variadics as well, and is the load that should be used.



			//In this case, param instructs (want a better name) are the instructions that exist on the parameter, for which shall be looped when handled


			Operand vard_index{ index, OperandType::Index };
			constexpr Operand none{ 0, OperandType::None };
			constexpr Operand zero{ -1, OperandType::Differ };
			constexpr Operand neg_one{ -1, OperandType::Differ };
			constexpr Operand pos_one{ 1, OperandType::Differ };
			constexpr Operand reg_left{ Register::Left, OperandType::Register };
			constexpr Operand reg_right{ Register::Right, OperandType::Register };

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			Register reg = compiler->GetPrefered();

			Operand pref{ reg, OperandType::Register };

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			LocalInfo* tmp = compiler->GetScope()->ObtainLocalVariable(parse_strings::variadic_buffer);

			Operand buffer{ tmp->GetIndex(), OperandType::Value };

			//Allocate based on the location of the vard parameter, and move the value to the buffer
			//result.push_back(Instruction{ InstructType::VardAlloc, buffer, vard_index });//I would like to face this out
			//~~~INSTEAD~~~
			result.push_back(Instruction{ InstructType::ExpressData, reg , Operand{ RuntimeData::VariadicLength, OperandType::Enum}, vard_index});
			result.push_back(Instruction{ InstructType::Transfer, buffer, pref });


			auto query_index = result.size();

			result.push_back(Instruction{ InstructType::DropStack, zero, buffer });

			result.push_back(Instruction{ InstructType::AssignModValue, buffer, neg_one });	//Deccrement vard index

			result.append_range(express_instructs);//Load the arguments that will have 

			if (func)
			{
				RoutineCompiler::TempListHandle handle{ result, compiler->_current };
				from = func(from);
			}



			//result.push_back(Instruction{ InstructType::ExpressConstant, none, vard_index, Register::Right });
			//result.push_back(Instruction{ InstructType::AssignModValue, buffer, reg_right });
			result.push_back(Instruction{ InstructType::ModifyValue, reg, buffer, vard_index });


			result.append_range(MutateLoad(from, pref.AsDynamic(DynamicIndex::Argument), is_ref));

			int64_t size = (int64_t)result.size();

			result.push_back(Instruction{ InstructType::DropStack, Operand{-(size - query_index), OperandType::Differ}, pos_one });

			size = (int64_t)result.size();

			auto& query = result[query_index];


			query._lhs = Target{ size - query_index };
		}

		compiler->AppendInstructions(record, result);
	}


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

			if (out.HandleInstruction(compiler, target, value, to, res, reg) == false)
				return false;

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