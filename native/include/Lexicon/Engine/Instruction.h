#pragma once

#include "Target.h"
#include "Operand.h"
#include "OperandType.h"
#include "Operation.h"
#include "InstructionType.h"

namespace LEX
{

	struct Instruction final
	{
		struct RecordIndex
		{
			constexpr RecordIndex() noexcept = default;
			constexpr RecordIndex(uint32_t i)
			{
				if (i >= k_maxIndex) {
					value = k_invalid;
					return;
				}

				value[0] = (i & 0x0000FF);
				value[1] = (i & 0x00FF00) << 8;
				value[2] = (i & 0xFF0000) << 16;
			}

			operator uint32_t () const
			{
				if (k_invalid == value) {
					return -1;
				}

				return (value[0]) | (value[1] << 8) | (value[2] << 16);
			}


			static constexpr uint32_t k_maxIndex = 0x00FFFFFF;
			constexpr static std::array<uint8_t, 3> k_invalid{ 255, 255, 255 };

			std::array<uint8_t, 3> value = k_invalid;
		};



		//Main constructor, Takes 2 Operands left and right, an out register, and instruction.
		//The instruction and register are first, because usually they're non optional.

		constexpr Instruction() = default;
		//switch order, reg last
		constexpr Instruction(InstructionType it, Register reg = Register::Invalid, Operand left = {}, Operand right = {}, uint32_t i = -1) :
			_instruct{ it }, _out{ reg },
			_lhs{ left }, _ltype{ left.type },
			_rhs{ right }, _rtype{ right.type },
			index{ i }
		
		{

		}

		//remove when above is fixed
		constexpr Instruction(InstructionType it, Operand left, Operand right = {}, uint32_t i = -1) :
			_instruct{ it },
			_lhs{ left }, _ltype{ left.type },
			_rhs{ right }, _rtype{ right.type }

		{

		}

		constexpr Instruction(InstructionType it, Operand left, uint32_t i) : Instruction{ it, left, {}, i }
		{

		}


		//might switch target and operand names
		Target _lhs{};//8
		Target _rhs{};//8

		OperandType _ltype = OperandType::None;//1
		OperandType _rtype = OperandType::None;//1	

		InstructType _instruct = InstructType::Invalid;//2?

		Register _out = Register::Invalid;//1

		//This exists as an index for "failable action". Will stop recording at a certain point, but basically along side of compilation, should be a list of record pointers
		// which allow one to get the line and column of its failure. Will not be used for something that wasn't given a place to allocate record pointers to. Also, if it runs out of
		// space it will just stop debugging entirely.
		//The record code is really just an integer.
		
		RecordIndex index{};

		/*
		std::array<uint8_t, 3> recordCode{ 255, 255, 255 };

		//Want to clean the below up bit this is fine for now

		static constexpr uint32_t maxCode = 0x00FFFFFF;

		//Instead of this, I could possibly use a different operation to handle this. That starts in reference to a record, and ends in reference to a record.
		// which basically just tells you where you were at. It's a thing to think about, especially with formulas.
		bool SetRecordCode(uint32_t value)
		{
			if (value >= maxCode)
				return false;

			recordCode[0] = (value & 0x0000FF);
			recordCode[1] = (value & 0x00FF00) << 8;
			recordCode[2] = (value & 0xFF0000) << 16;

			//byte0 + (byte1 << 8) + (byte2 << 16);

			return true;
		}

		uint32_t GetRecordCode()
		{
			return recordCode[0] | (recordCode[1] << 8) | (recordCode[2] << 16);
		}
		//*/
		Operand GetTarget(bool left)
		{
			return left ? Operand{ _lhs, _ltype } : Operand{ _rhs, _rtype };
		}

		void Execute(Runtime* runtime);

	};
	REQUIRED_SIZE(Instruction, 0x18);
}