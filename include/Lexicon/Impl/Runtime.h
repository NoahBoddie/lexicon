#pragma once

#include "IGenericArgument.h"
#include "ICallableUnit.h"
#include "RuntimeVariable.h"
#include "RoutineBase.h"
#include "Register.h"

//To be in impl probably.
namespace LEX
{

	template<typename T>
	using container = std::vector<T>;
	//using container = std::span<T, std::dynamic_extent>;


	struct RuntimeFlag// : public ClassAlias<std::bitset<8>>
	{
		//ALIAS_HEADER;

		enum Flag : uint8_t
		{
			Bit1,	//A condition flag.
			//Unused
			Bit2,
			Bit3,
			Bit4,
			Bit5,
			Bit6,
			Bit7,
			Bit8,	//Is testing. Used when I mean to say, don't do in earnest.

			//NamedFlags
			TestBit = Bit1,

			IncBit = Bit7,
			RetBit = Bit8
			//E
		};

		uint8_t _bits{ 0 };

		void Set(Flag flg, bool value)
		{

			//Think you actually use XOR or something but I don't super care.
			if (value)
				_bits &= ~_bits | (1 << (uint8_t)flg);

			if (value)
				_bits &= ~(_bits & (1 << (uint8_t)flg));
		}

		bool Get(Flag flg)
		{
			return _bits & (1 << (uint8_t)flg);
		}

		bool Pop(Flag flg)
		{
			bool result = Get(flg);

			if (result)
				Set(flg, false);

			return result;
		}
	};

	enum struct StackPointer
	{
		Variable,
		Argument,
		Runtime
	};

	struct Runtime : public GenericArgumentArray
	{
		//TODO:Make sure RoutineProcess can be copied for testing the return value.
		//Target could use variable instead of an object. Worth thinking about sorta?


		//The idea is that the callable unit is given it's parameters
		Runtime(ICallableUnit* unit, container<RuntimeVariable>& args, Runtime* from = nullptr) :
			_data{ *unit->GetRoutine() }
			, _varStack{ _data.GetVarCapacity() }
			, _argStack{ _data.GetArgCapacity() }
			, _psp{ args.size() }
			, _vsp{ _psp }
			, _caller{ from }
		{
			//This should actually do some sanity checks???
			return;
		}

		//Very temporary, delete me
		Runtime(RoutineBase& base) :
			_data{ base }
			, _varStack{ _data.GetVarCapacity() }
			, _argStack{ _data.GetArgCapacity() }
			, _psp{ 0 }
			, _vsp{ 0 }
			, _caller{ nullptr }
		{
			return;
		}



		RoutineBase& _data;

		std::array<RuntimeVariable, Impl::Register::Total> _registers;

		RuntimeFlag _flags{};
		//Free 7 bytes. or more flags who knows. I could make a set of user defined flags, but I wouldn't know what to use them for that a variable wouldn't suffice
		// so I'll drop it.


		Runtime* _caller = nullptr;//Does parent even matter anymore? that was for subarithmetic within


		//I'm not actually sure this goes here if I'm being honest.
		//AbstractTypePolicy* specialization;

		//Constant


		container<RuntimeVariable> _varStack;//A certain point should ONL
		container<RuntimeVariable> _argStack;
		size_t _psp{ 0 };		//Param Stack Pointer, denotes the last position of a parameter within the variable stack. Entries after containing ref should crash. Constant?
		//I don't want to do this in release. UNLESS, one of the variables has changed. Maybe I can avoid this by controlling HOW something gets a reference
		// to runtime variables?

		size_t _vsp{ 0 };		//Variable stack pointer, denotes the current size of the variable. Being smaller than PSP should crash.
		size_t _asp{ 0 };		//Argument StackPointer, denotes the current size of the argument stack. A function exiting with it !0 should crash.
		size_t _rsp{ 0 };		//Runtime Stack Pointer, denotes the index at which the runtime is currently. If exceeds the operation count it should crash


		size_t AdjustStackPointer(StackPointer type, int64_t step)
		{
			//These might all need validation, just to make sure an issue doesn't occur.
			switch (type)
			{
			case StackPointer::Variable:
				//"Allocates" the variable stack, clearing whatever value was there last
				// Only works in forward
				for (auto i = _vsp; i < _vsp + step; i++) {
					_varStack[i].Ref().Clear();
				}
				return _vsp += step;

			case StackPointer::Argument:
				//"Deallocates" the argument stack only when the pointer is pushed down
				// Only works in backward
				for (auto i = _asp + step; i < _asp; i++) {
					_argStack[i].Ref().Clear();
				}
				return _asp += step;

			case StackPointer::Runtime:
				_flags.Set(RuntimeFlag::IncBit, true);
				return _rsp += step;
			}

			auto t = "Unknown stack pointer";
			throw nullptr;
		}


		size_t GetStackPointer(StackPointer type)
		{
			//These might all need validation, just to make sure an issue doesn't occur.
			switch (type)
			{
			case StackPointer::Variable:
				return _vsp;

			case StackPointer::Argument:
				return _asp;

			case StackPointer::Runtime:
				return _rsp;
			}

			auto t = "Unknown stack pointer";
			throw nullptr;
		}

		size_t SetStackPointer(StackPointer type, uint64_t index)
		{
			//These might all need validation, just to make sure an issue doesn't occur.
			switch (type)
			{
			case StackPointer::Variable:
				//No clue what the hell this does atm.

			case StackPointer::Argument:
				//Same with this.
				break;


			case StackPointer::Runtime:
				_flags.Set(RuntimeFlag::IncBit, true);
				return _rsp += index;
			}

			auto t = "Unknown stack pointer";
			throw nullptr;
		}

		//May be replaced in the coming times.
		RuntimeVariable& GetVariable(size_t i)
		{
			if (i >= _vsp)
				throw nullptr;

			return _varStack[i];
		}

		RuntimeVariable& GetArgument(size_t i)
		{
			//I'm thinking this should be input only, with a different function that gets a vector of the given arguments.
			//I feel this should be input only, at least mostly. Really want to revisit. Especially since I don't like GetVariable's setup.
			return _argStack[i];

		}

		//May be replaced in the coming times.
		RuntimeVariable& GetRegister(Impl::Register reg)
		{
			return _registers[reg];
		}

		RuntimeFlag GetFlags() const
		{
			return _flags;
		}


		void Operate(Operation& operation)
		{
			//GetTarget- use this instead.

			/*
			Variable result;//Unsure if this is advized.

			instructList[operation._instruct].Operate(result, this, operation._lhs, operation._rhs);//It's possible that this should possibly return.
			//If not void and complete (or just complete because void isn't a complete type
			//If it's void, that's fine
			if (operation._out != Register::Invalid && result.IsVoid() == false)
				GetVariable(operation._out);
			//*/
		}

		//TODO:Make a static function do Run and construct, no need to wait right? Doing so means I can also make a function called "Test"
		RuntimeVariable Run()
		{
			if (_rsp != max_value<size_t>)
			{
				//I'm unsure if I'll want to move this to being  member or something.
				size_t _limit = _data.GetOperativeCapacity();

				while (_flags.Get(RuntimeFlag::RetBit) == false)
				{
					if (_rsp >= _limit)
						throw nullptr;

					//Probably want to do a try catch around here.
					Operate(_data[_rsp]);
					_data[_rsp].Execute(this);
					if (_psp > _vsp)
						throw nullptr;

					if (_flags.Pop(RuntimeFlag::IncBit) == true) {
						_rsp++;
					}
				}

				if (_asp) {//If not the value it started as (zero) something was called incorrectly. This is a proper crash.
					throw nullptr;
				}

				_rsp = max_value<size_t>;
			}

			//return _registers[Register::Reg0];
			return _registers[0];


			//Old version. Not sure if this is the way I want to do it, so I can catch possible unintended returns.
			/*
			while (_rsp < _limit)
			{
				auto rember = _index;
				//Access operate this. Better for exceptions and such.
				//_data.operations[_index].Operate(this);
				Operate(_data.operations[_index]);

				if (_index == rember)
					_index++;
			}

			return _registers[Register::Reg0];
			//*/
		}

		//THIS is the gist of what I'd like.
		static RuntimeVariable Run(ICallableUnit* unit, container<RuntimeVariable> args, Runtime* from = nullptr)
		{
			Runtime call_runtime{ unit, args, from };

			return call_runtime.Run();
		}
	};

}