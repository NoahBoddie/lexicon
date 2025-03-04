#pragma once

#include "Lexicon/Engine/Solution.h"
#include "Lexicon/Engine/Operation.h"
#include "Lexicon/Engine/SyntaxRecord.h"
namespace LEX
{
	struct RoutineCompiler;
	struct ExpressionCompiler;


	struct CompUtil
	{
		//A set convenience functions.


	private:
		static Operation _Transfer(Operand& left, Operand& right, bool is_ref)
		{
			return Operation{ false ? InstructType::Reference : InstructType::Copy, left, right };
		}
	public:

		static Operation Transfer(Operand& left, Solution& right)
		{
			//The left can be an operand because it doesn't quite matter what it does, we just need to know
			// where it's to go. That check should be handled, before hand.

			bool is_ref = true;

			if (right.type == OperandType::Literal) {
				//Literals can never be passed around by ref, so by default, it has to be moved, not forwarded.
				is_ref = false;
			}


			return _Transfer(left, right, is_ref);
		}


		static Operation Transfer(Solution& left, Solution& right)
		{
			//If the left is a reference already, it's not allowed to be overriden.
			if (left.IsReference() == true)
				return _Transfer(left, right, false);

			return Transfer(static_cast<Operand&>(left), right);
		}



		static Operation Transfer(Operand&& left, Solution& right)
		{
			return Transfer(static_cast<Operand&>(left), right);
		}


		static Operation Transfer(Solution&& left, Solution& right)
		{
			return Transfer(static_cast<Solution&>(left), right);
		}




		//Changes the solution to be where the operand is, and gives the operation that would make it where it's expected.
		[[nodiscard]] static Operation Mutate(Solution& sol, Operand op)
		{
			//Result shouldn't be discarded, because the solution is being change to reflect the operation.

			//Using transfer ensures safety for literals.
			Operation result = CompUtil::Transfer(op, sol);

			//This will set what operand stores into the solution without touching the type data within
			sol = op;


			return result;
		}

		//Changes the solution to be where the operand is similar to mutate, but forces it to copy instead.
		[[nodiscard]] static Operation MutateCopy(Solution& sol, Operand op)
		{
			//Result shouldn't be discarded, because the solution is being change to reflect the operation.

			Operation result = Operation{ InstructType::Copy, op, sol };

			sol = op;

			return result;
		}



		//Changes the solution to be where the operand is similar to mutate, but forces it to ref instead. A temporary function to sus out issue with member accesses.
		[[nodiscard]] static Operation MutateRef(const Solution& sol, Operand op)
		{
			//Result shouldn't be discarded, because the solution is being change to reflect the operation.
			
			return Operation{ InstructType::Reference, op, sol };
		}


		[[nodiscard]] static Operation MutateRef(Solution& sol, Operand op)
		{
			//Result shouldn't be discarded, because the solution is being change to reflect the operation.

			Operation result = MutateRef(std::as_const(sol), op);

			sol = op;

			return result;
		}
		[[nodiscard]] static Operation MutateRef(Solution&& sol, Operand op)
		{
			//Result shouldn't be discarded, because the solution is being change to reflect the operation.

			Operation result = MutateRef(std::as_const(sol), op);

			return result;
		}


		static bool HandleConversion(ExpressionCompiler* compiler, Conversion& out, Solution& value, const QualifiedType& to, ConvertResult res, Register reg = Register::Invalid);
		
		static bool HandleConversion(ExpressionCompiler* compiler, Solution& from, const QualifiedType& to, SyntaxRecord& target, Register reg = Register::Invalid, ConversionFlag flags = ConversionFlag::None);
		
		static bool HandleConversion(ExpressionCompiler* compiler, Solution& from, const QualifiedType& to, SyntaxRecord& target, ConversionFlag flags)
		{
			return HandleConversion(compiler, from, to, target, Register::Invalid, flags);
		}


		static void PrepareReturn(ExpressionCompiler* compiler, QualifiedType return_type, Solution value);


		static int64_t SkipScope(RoutineCompiler* compiler, const Operand& query, bool negate, uint64_t offset = 0);
	};

}