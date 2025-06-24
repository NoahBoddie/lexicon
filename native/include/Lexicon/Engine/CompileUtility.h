#pragma once

#include "Lexicon/Engine/Solution.h"
#include "Lexicon/Engine/Instruction.h"
#include "Lexicon/Engine/SyntaxRecord.h"

namespace LEX
{
	struct RoutineCompiler;
	struct ExpressionCompiler;
	
	using InstructList = std::vector<Instruction>;

	struct CompUtil
	{
		//A set convenience functions.

		/*
	private:
		//There are some instances where transfer cannot be used, namely with references. Or rather when initing them, 
		// transfers job is passing values, not passing references
		static Instruction _Transfer(const Operand& left, Operand& right, std::optional<bool> is_ref)
		{
			return Instruction{ false ? InstructType::Reference : InstructType::Copy, left, right };
		}

	public:

		static Instruction Transfer(const Operand& left, Solution& right)
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


		static Instruction Transfer(Solution& left, Solution& right)
		{
			//If the left is a reference already, it's not allowed to be overriden.
			if (left.IsReference() == true)
				return _Transfer(left, right, false);

			return Transfer(static_cast<Operand&>(left), right);
		}

		//This should be used during initialization
		static Instruction Initialize(const Solution& left, Solution& right, SyntaxRecord* from)
		{
			InstructType  type;

			if (auto refl = left.IsReference(); !refl.has_value()) {
				//Maybe ref
				return Instruction{ right.IsTemporary() ? InstructType::ForwardMove : InstructType::Forward, left, right };
			}
			else if (refl.value_or(true) == true)
			{
				//Hard refs
				if (left.IsReference().value_or(false) == false) {
					//This call should be safe, forgoing the syntax record if "this" doesn't exist
					from->error("cannot initialize reference with a non-reference value");
				}

				return Instruction{ InstructType::Reference, left, right };
				
			}
			else
			{
				//no ref
				return Transfer(left, right);	
			}
		}






		//Changes the solution to be where the operand is, and gives the operation that would make it where it's expected.
		[[nodiscard]] static Instruction Mutate(Solution& sol, Operand op)
		{
			//Result shouldn't be discarded, because the solution is being change to reflect the operation.

			//Using transfer ensures safety for literals.
			Instruction result = CompUtil::Transfer(op, sol);

			//This will set what operand stores into the solution without touching the type data within
			sol = op;


			return result;
		}

		//Changes the solution to be where the operand is similar to mutate, but forces it to copy instead.
		[[nodiscard]] static Instruction MutateCopy(Solution& sol, Operand op)
		{
			//Result shouldn't be discarded, because the solution is being change to reflect the operation.

			//When the fuck is this supposed to be used?

			Instruction result = Instruction{ InstructType::Copy, op, sol };

			sol = op;

			return result;
		}


		//TODO: Mutate ref does NOT need all this shit.

		//Changes the solution to be where the operand is similar to mutate, but forces it to ref instead. A temporary function to sus out issue with member accesses.
		[[nodiscard]] static Instruction MutateRef(const Solution& sol, Operand op)
		{
			//Result shouldn't be discarded, because the solution is being change to reflect the operation.
			
			return Instruction{ InstructType::Reference, op, sol };
		}


		[[nodiscard]] static Instruction MutateRef(Solution& sol, Operand op)
		{
			//Result shouldn't be discarded, because the solution is being change to reflect the operation.

			Instruction result = MutateRef(std::as_const(sol), op);

			sol = op;

			return result;
		}
		[[nodiscard]] static Instruction MutateRef(Solution&& sol, Operand op)
		{
			//Result shouldn't be discarded, because the solution is being change to reflect the operation.

			Instruction result = MutateRef(std::as_const(sol), op);

			return result;
		}

		/*/



		
		static Instruction Transfer(const Operand& left, const Solution& right)
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
			return Instruction{ instruct, left, right };
		}


		static std::vector<Instruction> Load(const Operand& left, const Solution& right, std::optional<bool> is_ref)
		{
			std::vector<Instruction> result;

			//return Instruction{ InstructType::Copy, left, right };
			if (!left.IsTemporary() && right.IsPromoted() == true) {
				//Unsure if the left temporary is a necessary check.
				result.emplace_back(InstructType::Promote, right, Operand{ right.reference, OperandType::Index });
			}



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
			result.emplace_back(instruct, left, right);
			
			return  result;
			return  { Instruction{ instruct, left, right } };
		}

		static std::vector<Instruction> Load(const Solution& left, const Solution& right)
		{
			return Load(left, right, left.IsReference());
		}





		static Instruction Transfer(Register reg, const Solution& right)
		{
			return Transfer(Operand{ reg, OperandType::Register }, right);
		}








		//Changes the solution to be where the operand is, and gives the operation that would make it where it's expected.
		[[nodiscard]] static Instruction Mutate(Solution& sol, const Operand& op)
		{
			Instruction result = CompUtil::Transfer(op, sol);
			sol = op;
			return result;
		}

		[[nodiscard]] static Instruction Mutate(Solution& sol, Register reg)
		{
			Instruction result = CompUtil::Transfer(reg, sol);

			sol = Operand{ reg, OperandType::Register };

			return result;
		}

		//May move this, it takes a solution and it's reported going to location, and gets what it should be treated as. 
		// Say in a place where it copies, it will become modable, since it will be copied.
		static Solution Sanitize(const Solution& from, const Qualifier& to)
		{
			return from;
		}


		//Changes the solution to be where the operand is similar to mutate, but forces it to copy instead.
		[[nodiscard]] static Instruction MutateCopy(Solution& sol, const Operand& op)
		{
			//Result shouldn't be discarded, because the solution is being change to reflect the operation.

			//When the fuck is this supposed to be used?

			Instruction result = Instruction{ InstructType::Copy, op, sol };

			sol = op;

			return result;
		}

		//Changes the solution to be where the operand is similar to mutate, but forces it to copy instead.
		[[nodiscard]] static Instruction MutateCopy(Solution& sol, Register reg)
		{
			return MutateCopy(sol, Operand{ reg, OperandType::Register });
		}




		//TODO: Mutate ref does NOT need all this shit.

		//Changes the solution to be where the operand is similar to mutate, but forces it to ref instead. A temporary function to sus out issue with member accesses.
		[[nodiscard]] static Instruction MutateRef(Solution& sol, const Operand& op)
		{
			Instruction result = Instruction{ InstructType::Reference, op, sol };
			sol = op;
			return result;
		}

		[[nodiscard]] static Instruction MutateRef(Solution& sol, Register reg)
		{
			return MutateRef(sol, Operand{ reg, OperandType::Register });
		}





		static std::vector<Instruction> MutateLoad(Solution& sol, const Operand& to, std::optional<bool> is_ref)
		{
			auto result = Load(to, sol, is_ref);
			sol.ClearPromotion();
			sol = static_cast<const Operand&>(to);
			return result;
		}



		[[nodiscard]] static std::vector<Instruction> MutateLoad(Solution& sol,  const Solution& to)
		{
			return MutateLoad(sol, to, to.IsReference());
		}
		
		


		static Instruction RemoveOperation(std::vector<Instruction>& ops, InstructList::iterator it)
		{
			bool before = true;


		}

		static int GetStackOpDirection(const Instruction& op, size_t loc)
		{
			int64_t distance;

			switch (op._instruct)
			{
			//The more I think about it, JumpStack doesn't make sense for how this compiles does it?
			//case InstructType::JumpStack:
			//	distance = loc - op._lhs.index;
			//	break;

			case InstructType::DropStack:
			case InstructType::DropStackN:
				distance = op._lhs.differ;
				break;

			default:
				distance = 0;
			}

			return std::clamp<Differ>(op._lhs.differ, -1, 1);
		}

		static size_t RemoveOperations(std::vector<Instruction>& ops, std::function<bool(InstructList::iterator)> function)
		{
			//The idea is this function records relevant

			std::vector<InstructList::iterator> locDown;//This is for any jump or jump like operation that points to a place downwards (is positive)
			std::vector<InstructList::iterator> locUp;	//For any jump or jump like operation that points upwards (is positive).

			
			size_t removed = 0;
			bool erased;
			std::function<void(InstructList::iterator)> EraseEntry = [&](InstructList::iterator it)
			{
				it = ops.erase(it);
				erased = true;
				removed++;
				
				auto i = locDown.begin();
				
				while ( i != locDown.end())
				{
					auto loc = *i;

					if (--loc->_lhs.differ == 0){
						i = locDown.erase(i);

						EraseEntry(loc);//After this happens, the locUps need to be updated as well
					}
					else {
						i++;
					}
				}
			};

			std::function<void(InstructList::iterator)> DecUpLoc = [&](InstructList::iterator it)
			{
				it->_lhs.differ -= removed;
				locUp.push_back(it);
			};


			size_t i = 0;

			for (auto it = ops.begin(); it != ops.end(); i++, erased ? it : it++)
			{
				//The idea is if the jump is pointing down, we add it to downward jumps, which get decremented everything something is removed.
				// if it's pointing up, we reduce it by the number of removed operations.
				// If the jump distance has been made 0, I want a way to make it drop.

				if (function(it) == true) {
					
					EraseEntry(it);
					continue;
				}
				
				erased = false;
			
				auto direction = GetStackOpDirection(*it, i);

				if (direction > 0) {
					locDown.push_back(it);
				}
				else if (direction < 0) {
					DecUpLoc(it);
				}

			}

			return removed;
		}

		[[nodiscard]] static size_t MergeIncrement(std::span<std::vector<Instruction>> list, bool argument)
		{
			size_t result = 0;

			InstructType type = argument ? InstructType::ModArgStack : InstructType::ModVarStack;


			for (auto& ops : list)
			{

				RemoveOperations(ops, [&](InstructList::iterator it)->bool
				{
					if (it->_instruct == type) {
						result += std::max<Differ>(it->_lhs.differ, 0);
						return true;
					}

					return false;
				});
			}

			return result;
		}

		[[nodiscard]] static size_t MergeIncrement(std::vector<Instruction>& ops, bool argument)
		{
			return MergeIncrement({ &ops, 1 }, argument);
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