#pragma once

#include "Lexicon/ITemplateBody.h"
#include "Lexicon/ICallableUnit.h"
#include "Lexicon/RuntimeVariable.h"
#include "Lexicon/Engine/RoutineBase.h"
#include "Lexicon/Engine/Register.h"
#include "Lexicon/IRuntime.h"
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
				_bits |= 1 << (uint8_t)flg;

			else
				_bits &= ~(1 << (uint8_t)flg);
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
	
	class Runtime : public IRuntime, public ITemplateBody
	{
		//TODO:Make sure RoutineProcess can be copied for testing the return value.
		//Target could use variable instead of an object. Worth thinking about sorta?

	public:

		IRuntime* GetParent() const override
		{
			return _caller;
		}


		const Function* GetFunction() const override
		{
			return _function;
		}

		RuntimeVariable* GetDefault() const override
		{
			return nullptr;
		}


		//These will honestly do nothing for a long time, but I'll make them at some point.
		Column GetColumn() const override
		{
			return 0;
		}
		virtual Line GetLine() const override
		{
			return 0;
		}
		virtual std::string_view GetFile() const override
		{
			return "<No file found>";
		}

		ITemplateBody* AsBody() override
		{
			return this;
		}


		/*
		void Message(const std::string_view& message, IssueLevel level = IssueLevel::Trace, std::source_location loc = std::source_location::current()) const
		{

			logger::info("");
			if (_data[_rsp].index == -1) {
				report::warn("no record for logging or something");
			}

			_records->Log(std::string{ message }, loc, level, _data[_rsp].index);

		}
		//*/

		void Log(std::source_location loc, IssueLevel level, const std::string_view& message) const override
		{
			
			if (_data[_rsp].index == -1) {
				report::warn("no record for logging or something");
			}

			_records->Log(std::string{ message }, loc, level, _data[_rsp].index);
		}




		virtual size_t GetSize() const override
		{
			return _tempBody ? _tempBody->GetSize() : 0;
		}


		
		ITypeInfo* GetPartArgument(size_t i) const override
		{
			return _tempBody ? _tempBody->GetPartArgument(i) : nullptr;
		}

		TypeInfo* GetBodyArgument(size_t i) const override
		{
			return _tempBody ? _tempBody->GetBodyArgument(i) : nullptr;
		}

	public:

		~Runtime()
		{
			//
			AdjustStackPointer(StackPointer::Variable, -_vsp);
		}


		//The idea is that the callable unit is given it's parameters
		/*
		Runtime(ICallableUnit* unit, container<RuntimeVariable>& args, Runtime* from = nullptr) :
			_data{ *unit->GetRoutine() }
			//These accidently create numbers.
			//, _varStack{ _data.GetVarCapacity() }
			//, _argStack{ _data.GetArgCapacity() }
			//, _psp{ args.size() }
			//, _vsp{ _psp }
			, _caller{ from }
		{
			//This should actually do some sanity checks???
			_varStack.resize(_data.GetVarCapacity());
			_argStack.resize(_data.GetArgCapacity());

			_varStack.shrink_to_fit();
			_argStack.shrink_to_fit();

			unit->ResolveArguments(args);
			_psp = _vsp = args.size();
			return;
		}
		//*/
		//Very temporary, delete me
		Runtime(RoutineBase& base, Function* function = nullptr, std::span<RuntimeVariable> args = {}, Runtime* caller = nullptr, ITemplateBody* body = nullptr) :
			_function{ function }
			, _data{ base }
			, _records { &base }
			//These accidently create numbers.
			//, _varStack{ _data.GetVarCapacity() }
			//, _argStack{ _data.GetArgCapacity() }
			//, _psp{ 0 }
			//, _vsp{ 0 }//The size should actually be based on the callable unit
			, _tempBody { body }
			, _caller{ caller }
		{
			//because this is temp, no resolution is fired.

			_varStack.resize(_data.GetVarCapacity());
			_argStack.resize(_data.GetArgCapacity());

			_varStack.shrink_to_fit();
			_argStack.shrink_to_fit();
			
			

			if (args.size() != 0)
			{
				if (_varStack.size() < args.size())
					report::fault::critical("the size of the var stack is less tha the size of the arguments given.");


				//std::copy(_varStack.begin(), _varStack.begin(), args.begin());
				for (int i = 0; i < args.size(); i++)
				{
					//TODO:This aspect should probably be handled by the scripts, copying anything that should be copied instead of just referencing.s
					_varStack[i] = args[i];
				}
			}

			_psp = _vsp = args.size();
		}
	


		//
		Function* _function = nullptr;

		RoutineBase& _data;

		RecordHolder* _records = nullptr;

		std::array<RuntimeVariable, Register::Total> _registers;

		RuntimeFlag _flags{};
		//Free 7 bytes. or more flags who knows. I could make a set of user defined flags, but I wouldn't know what to use them for that a variable wouldn't suffice
		// so I'll drop it.


		Runtime* _caller = nullptr;//Does parent even matter anymore? that was for subarithmetic within


		//I'm not actually sure this goes here if I'm being honest.
		//TypeInfo* specialization;

		//Constant


		container<RuntimeVariable> _varStack;//A certain point should ONL
		container<RuntimeVariable> _argStack;
		
		//This already contains a generic argument array, I need to figure out how it's used.
		//container<ITypeInfo*> _genStack;

		int64_t _psp{ 0 };		//Param Stack Pointer, denotes the last position of a parameter within the variable stack. Entries after containing ref should crash. Constant?
		//I don't want to do this in release. UNLESS, one of the variables has changed. Maybe I can avoid this by controlling HOW something gets a reference
		// to runtime variables?

		int64_t _vsp{ 0 };		//Variable stack pointer, denotes the current size of the variable. Being smaller than PSP should crash.
		int64_t _asp{ 0 };		//Argument StackPointer, denotes the current size of the argument stack. A function exiting with it !0 should crash.
		int64_t _rsp{ 0 };		//Runtime Stack Pointer, denotes the index at which the runtime is currently. If exceeds the operation count it should crash

		

		//This will help check if something was stack allocated, and if that stack allocated 
		// object is being passed by reference some place it shouldn't be.
		// Expand on when references become more important, as well as give it a fancy type to aide its struggles
		void* _cxxStackIndex = nullptr;

		ITemplateBody* _tempBody = nullptr;

		size_t AdjustStackPointer(StackPointer type, int64_t step)
		{

			//Clear never fires on this worth a worry.

			//These might all need validation, just to make sure an issue doesn't occur.
			switch (type)
			{
			case StackPointer::Variable:
				//"Allocates" the variable stack, clearing whatever value was there last
				// Only works in forward
				//for (auto i = _vsp + step; i >= _vsp; i--) {
				//	if (auto& var = _varStack[i]; var.IsVoid() == false)
				//		//var->Clear();
				//		var.Clear();//Clears the runtime var instead of the value it targets
				//}
				//return _vsp += step;
				
				for (auto i = _vsp - 1; i >= _vsp + step; i--) {
					if (auto& var = _varStack[i]; var.IsVoid() == false) {
						//var->Clear();
						var.Clear();//Clears the runtime var instead of the value it targets
					}
				}
				return _vsp += step;

			case StackPointer::Argument:
				//"Deallocates" the argument stack only when the pointer is pushed down
				// Only works in backward
				//*Also, this looks like it will not work.
				
				//for (auto i = _asp; i >= _asp + step; i--) {
				//	//_argStack[i]->Clear();
				//	_argStack[i].Clear();
				//}
				//return _asp += step;
				for (auto i = _asp + step; i < _asp; i++) {
					//_argStack[i]->Clear();
					_argStack[i].Clear();
				}
				return _asp += step;

			case StackPointer::Runtime:
				_flags.Set(RuntimeFlag::IncBit, true);
				return _rsp += step;
			}

			report::runtime::critical("Unknown stack pointer");
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

			report::runtime::critical("Unknown stack pointer");
			return {};
		}

		size_t SetStackPointer(StackPointer type, uint64_t index)
		{
			//TODO: Rename SetStackPointer to ModStackPointer
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

			report::runtime::error("Unknown stack pointer type");  return {};
		}

		//May be replaced in the coming times.
		RuntimeVariable& GetVariable(size_t i)
		{
			if (i >= _vsp) {
				report::runtime::critical("Variable stack index larger than current stack size. ({}/{})", i, _vsp);
			}

			return _varStack[i];
		}

		RuntimeVariable& GetArgument(size_t i)
		{
			if (i >= _asp) {
				report::runtime::critical("Argument stack index larger than current stack size. ({}/{})", i, _asp);
			}

			//I'm thinking this should be input only, with a different function that gets a vector of the given arguments.
			//I feel this should be input only, at least mostly. Really want to revisit. Especially since I don't like GetVariable's setup.
			return _argStack[i];

		}

		RuntimeVariable& GetArgumentFromBack(size_t i)
		{
			int64_t index = _asp - i;
			
			if (index < 0) {
				report::runtime::critical("Argument stack back index larger than current stack size(make more unique). ({}/{})", i, _asp);
			}
			
			return GetArgument(index);

		}

		std::vector<RuntimeVariable> GetArgsInRange(size_t i)
		{
			if (!i)
				return {};

			//error should happen depending the stack size and what value ASP is.


			auto begin = _argStack.begin() + (_asp - i);
			auto end = _argStack.begin() + _asp;


			std::vector<RuntimeVariable> result;
			
			result.resize(i);

			std::transform(end - i, end, result.begin(), [&](RuntimeVariable& it) {return it.AsRef(); });

			//std::vector<RuntimeVariable> result{ end + i, end };

			return result;
		}

		//May be replaced in the coming times.
		RuntimeVariable& GetRegister(Register reg)
		{
			return _registers[reg];
		}

		RuntimeFlag GetFlags() const
		{
			return _flags;
		}



		Runtime* GetCaller() const noexcept
		{
			return _caller;
		}

		bool ContainsVariable(const RuntimeVariable& var) const
		{
			//This needs to actually test if it's made on the stack
			if (var.IsRuntimeRef() == false) {
				return false;
			}

			const void* ptr = var.Ptr();

			//unsure if I should do it here
			//for (auto& reg : _registers)
			//{
			//	if (reg.Ptr() == )
			//}
			
			//for now I'm just doing var checks

			for (auto& obj : _varStack)
			{
				if (obj.IsValue() && obj.Ptr() == ptr)
					return true;
			}
			
			return false;
		}

		bool OwnsVariable(const RuntimeVariable& var) const
		{
			const Runtime* runtime = this;
			bool result = false;

			while (runtime && !result)
			{
				result = runtime->ContainsVariable(var);
				runtime = runtime->GetCaller();
			}
			
			return result;
		}


		//TODO:Make a static function do Run and construct, no need to wait right? Doing so means I can also make a function called "Test"
		RuntimeVariable Run(bool temp = false)
		{
			//TODO: An object to manage this would be great
			_cxxStackIndex = &temp;

			report _{ IssueType::Runtime };

			size_t _limit = _data.GetInstructCapacity();
			
			if (_limit == 0)
				return {};

			size_t loop_no = 1;

			if (_rsp != max_value<size_t>)
			{
				//I'm unsure if I'll want to move this to being  member or something.
				

				while (_flags.Get(RuntimeFlag::RetBit) == false)
				{
					report::runtime::trace("In loop {}, stack index at {}", loop_no++, _rsp);

					if (_rsp >= _limit) {
						report::runtime::critical("Runtime Stack Pointer equals or exceeds op size ({}/{}), terminating program.", _rsp, _limit);
					}

					//Probably want to do a try catch around here.
					//Operate(_data[_rsp]);
					//TODO: I want to make a break point here to simulate walking through the system, but enable and disable it somehow.
					Debug("Logging");
					_data[_rsp].Execute(this);
					

					if (_psp > _vsp) {
						report::runtime::critical("Param Stack Pointer greater than Var Stack Pointer ({}/{}), terminating program.", _psp, _vsp);
					}
						

					if (_flags.Pop(RuntimeFlag::IncBit) == false) {
						_rsp++;
					}
				}
				


				if (_asp) {
					//If not the value it started as (zero) something was called incorrectly. This is a proper crash.
					report::runtime::critical("Argument Stack Pointer not 0 (is {}), terminating program", _asp);
				}

				_rsp = max_value<size_t>;
			}
			
			//Handle garbage stack protection shit right here please.
			_cxxStackIndex = nullptr;

			//TODO: As is, this can possibly return garbage. This is a behaviour that needs to be curbed.

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
			report::apply::critical("This shit is NOT SUPPOSED TO BE USED RIGHT NOW");
			assert(false);

			//static_assert(false);
			
			//Runtime call_runtime{ unit, args, from };

			//return call_runtime.Run();

			return {};
		}
	};

}