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

		/*
	private:
		//There are some instances where transfer cannot be used, namely with references. Or rather when initing them, 
		// transfers job is passing values, not passing references
		static Operation _Transfer(const Operand& left, Operand& right, std::optional<bool> is_ref)
		{
			return Operation{ false ? InstructType::Reference : InstructType::Copy, left, right };
		}

	public:

		static Operation Transfer(const Operand& left, Solution& right)
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

		//This should be used during initialization
		static Operation Initialize(const Solution& left, Solution& right, SyntaxRecord* from)
		{
			InstructType  type;

			if (auto refl = left.IsReference(); !refl.has_value()) {
				//Maybe ref
				return Operation{ right.IsTemporary() ? InstructType::ForwardMove : InstructType::Forward, left, right };
			}
			else if (refl.value_or(true) == true)
			{
				//Hard refs
				if (left.IsReference().value_or(false) == false) {
					//This call should be safe, forgoing the syntax record if "this" doesn't exist
					from->error("cannot initialize reference with a non-reference value");
				}

				return Operation{ InstructType::Reference, left, right };
				
			}
			else
			{
				//no ref
				return Transfer(left, right);	
			}
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

			//When the fuck is this supposed to be used?

			Operation result = Operation{ InstructType::Copy, op, sol };

			sol = op;

			return result;
		}


		//TODO: Mutate ref does NOT need all this shit.

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

		/*/



		
		static Operation Transfer(const Operand& left, const Solution& right)
		{

			//When would transfer want to forward move?
			InstructType instruct;
#ifdef THE_SUGGESTED
			
			//For us to be able to move it, it must not be a reference
			if (right.IsTemporary() && right.IsReference().value_or(false) == true)
			{
				instruct = InstructType::Move;
			}
			else
			{
				instruct = InstructType::Copy;
			}
			//I think this may come up some
#else
			//The take away from this is when is it that we'll want to take a reference, or forward move
			if (left.IsTemporary())
			{
				//I think if left is a register and the left isn't, as long as it's referential we reference it
				if (right.IsTemporary())
				{
					if (right.IsReference().has_value() == false) {
						instruct = InstructType::ForwardMove;
					}
					else {
						instruct = InstructType::Move;
					}
				}
				else
				{
					instruct = InstructType::Reference;
				}
			}
			else
			{
				instruct = InstructType::Copy;
			}
#endif
			return Operation{ instruct, left, right };
		}


		static Operation Load(const Operand& left, const Solution& right, std::optional<bool> is_ref)
		{
			//return Operation{ InstructType::Copy, left, right };




			//Should probably use transfer when it's no longer important
			InstructType instruct;
			//As far as I can see, this is ever actually wanted here
			if (1 != 1 && left.IsTemporary())
			{
				//I think if left is a register and the left isn't, as long as it's referential we reference it
				if (right.IsTemporary())
				{
					if (right.IsReference().has_value() == false) {
						instruct = InstructType::ForwardMove;
					}
					else {
						instruct = InstructType::Move;
					}
				}
				else
				{
					instruct = InstructType::Reference;
				}
			}
			else
			{
				switch (is_ref.has_value())
				{
				default://normal path
					if (is_ref.value())//Referenceable
					{
						if (right.IsReferential())
							instruct = InstructType::Reference;
						else
							report::compile::error("Cannot convert non-reference type to reference type");
					}
					else
					{
						if (right.IsReferential())
							instruct = InstructType::Copy;
						else
							instruct = InstructType::Move;
					}
					break;

				case false://maybe path
					if (right.IsTemporary() && right.IsReference().has_value() == false)
						instruct = InstructType::ForwardMove;
					else
						instruct = InstructType::Forward;
					break;
				}

				//instruct = InstructType::Copy;
			}



			//If right is a maybe, forward
			//When trying to temporary locations and right is 

			return Operation{ instruct, left, right };
		}

		static Operation Load(const Solution& left, const Solution& right)
		{
			return Load(left, right, left.IsReference());
		}


		//This should be used during initialization
		static Operation LoadOld(const Solution& left, Solution& right, SyntaxRecord* from)
		{
			InstructType  type;

			if (auto refl = left.IsReference(); !refl.has_value()) {
				//Maybe ref
				return Operation{ right.IsTemporary() ? InstructType::ForwardMove : InstructType::Forward, left, right };
			}
			else if (refl.value_or(true) == true)
			{
				//Hard refs
				if (left.IsReference().value_or(false) == false) {
					//This call should be safe, forgoing the syntax record if "this" doesn't exist
					from->error("cannot initialize reference with a non-reference value");
				}

				return Operation{ InstructType::Reference, left, right };

			}
			else
			{
				//no ref
				return Transfer(left, right);
			}
		}




		static Operation Transfer(Register reg, const Solution& right)
		{
			return Transfer(Operand{ reg, OperandType::Register }, right);
		}








		//Changes the solution to be where the operand is, and gives the operation that would make it where it's expected.
		[[nodiscard]] static Operation Mutate(Solution& sol, const Operand& op)
		{
			Operation result = CompUtil::Transfer(op, sol);
			sol = op;
			return result;
		}

		[[nodiscard]] static Operation Mutate(Solution& sol, Register reg)
		{
			Operation result = CompUtil::Transfer(reg, sol);

			sol = Operand{ reg, OperandType::Register };

			return result;
		}



		//Changes the solution to be where the operand is similar to mutate, but forces it to copy instead.
		[[nodiscard]] static Operation MutateCopy(Solution& sol, const Operand& op)
		{
			//Result shouldn't be discarded, because the solution is being change to reflect the operation.

			//When the fuck is this supposed to be used?

			Operation result = Operation{ InstructType::Copy, op, sol };

			sol = op;

			return result;
		}

		//Changes the solution to be where the operand is similar to mutate, but forces it to copy instead.
		[[nodiscard]] static Operation MutateCopy(Solution& sol, Register reg)
		{
			return MutateCopy(sol, Operand{ reg, OperandType::Register });
		}




		//TODO: Mutate ref does NOT need all this shit.

		//Changes the solution to be where the operand is similar to mutate, but forces it to ref instead. A temporary function to sus out issue with member accesses.
		[[nodiscard]] static Operation MutateRef(Solution& sol, const Operand& op)
		{
			Operation result = Operation{ InstructType::Reference, op, sol };
			sol = op;
			return result;
		}

		[[nodiscard]] static Operation MutateRef(Solution& sol, Register reg)
		{
			return MutateRef(sol, Operand{ reg, OperandType::Register });
		}





		static Operation MutateLoad(Solution& sol, const Operand& to, std::optional<bool> is_ref)
		{
			Operation result = Load(to, sol, is_ref);
			sol = static_cast<const Operand&>(to);
			return result;
		}



		[[nodiscard]] static Operation MutateLoad(Solution& sol,  const Solution& to)
		{
			return MutateLoad(sol, to, to.IsReference());
		}

		//*/



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