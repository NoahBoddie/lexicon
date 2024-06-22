#pragma once




//#include "Object.h"
//#include "ObjectID.h"
//#include "ObjectIDMapper.h"

#include "Lexicon/Impl/Operand.h"


#include "Lexicon/Impl/Script.h"
#include "Lexicon/Impl/Project.h"
#include "Lexicon/Impl/ProjectManager.h"

#include "Lexicon/Impl/Exception.h"
//#include "Parser.h"
#include "Lexicon/Impl/ParserTest.h"


#include "Compiler.h"

#include "Lexicon/Impl/Variable.h"



//#include "SignatureManager.h"//suppressed for now

//This is a test space for this. I'd like to move it to a more fitting location.

//New
//*
#include "Instruction.h"
#include "InstructionType.h"
#include "OperatorType.h"
#include "Runtime.h"
#include "RuntimeVariable.h"
#include "IVariable.h"
#include "GlobalVariable.h"
#include "Literal.h"
#include "LiteralManager.h"
#include "Scope.h"
#include "Solution.h"
#include "RoutineBase.h"
#include "Operation.h"
#include "Operand.h"
#include "OperandType.h"
#include "Target.h"
#include "RuntimeVariable.h"
#include "MemberPointer.h"
#include "RoutineCompiler.h"

#include "External.h"
#include "ExternalHandle.h"
#include "ExternalManager.h"

#include "TypeID.h"


#include "ConcretePolicy.h"


#include "Lexicon/Impl/VariableType.h"

#include "ConcreteFunction.h"

#include "OverloadClause.h"




#include "Object.h"
#include "ObjectData.h"
#include "ObjectInfo.h"
#include "ObjectPolicy.h"
#include "ObjectPolicyHandle.h"
#include "ObjectPolicyManager.h"


#include "ObjectPoolData.h"





#include "Interface.h"
#include "InterfaceManager.h"
#include "InterfaceSingleton.h"
#include "Versioning.h"

#include "DeclareSpecifier.h"

#include "OverloadInput.h"

#include "Declaration.h"

#include "ProcedureHandler.h"
#include "Dispatcher.h"

namespace std
{
	template <class _Elem, class _Alloc>
	struct hash<vector<_Elem, _Alloc>>
	{
		_NODISCARD static size_t _Do_hash(const vector<_Elem, _Alloc>& _Keyval) noexcept {
			return _Hash_array_representation(_Keyval.data(), _Keyval.size());
		}
	};
}

//This is to be my method of hashing.
inline std::hash<std::vector<uint64_t>> hasher;

namespace LEX
{
	struct SourceObject
	{
		//This is an object that is used to represent foreign C++ objects attempting to interface with
		// the lexicon system

		void* object = nullptr;
		std::type_info* info = nullptr;


		//Might these need to decay?
		template <typename T>
		SourceObject(T& self) : object{ &self }, info{ &typeid(T) }
		{
		}

		template <typename T>
		SourceObject(T&& self) : info{ &typeid(T) }
		{
		}
	};


	//I'll need to be able to tell a type by value (this is basically the vtable like side of things)
	// and the return type which is the type you can see.

	//So for example, nothing will ever be a MagicItem, literally does not exist so you'll get its variable type. BUT, you can get it's return type




	struct CONCEPT__TypeID
	{
		//will be redesigning the new range based type id system that should allow for much less wasted space.

		//Type ids will not be the raw id anymore
	};

	struct CONCEPT__TypeCode
	{
		//Type code will continue to exist, this is what is sent along with a name in order to actually find a proper type
		// So that a proper object can be created.
		//This is 16 bit when it comes to that. But hold off on.
	};


	using __TypeID = uint32_t;


	/////////////////////////
	//Implementations
	////////////////////////



	struct CoreOffset
	{
		//CORE isn't a thing yet, but when it is this is what it'd be using.
		constexpr static TypeOffset Void = 0;
		constexpr static TypeOffset Number = 0;
		constexpr static TypeOffset String = 0;
		constexpr static TypeOffset Array = 0;
		constexpr static TypeOffset Function = 0;//FunctionHandle
		constexpr static TypeOffset Object = 0;//ExternalHandle
		constexpr static TypeOffset Delegate = 0;
	};

#define NUMBER_SET_NAME "NUMBER"
#define STRING_SET_NAME "STRING"
#define ARRAY_SET_NAME "ARRAY"
#define FUNCTION_SET_NAME "FUNCTION"
#define OBJECT_SET_NAME "OBJECT"
#define DELEGATE_SET_NAME "DELEGATE"

	//TODO: This should be moved.

	AbstractTypePolicy* VariableType<Void>::operator()(const Void*)
	{
		return nullptr;
	}



	AbstractTypePolicy* VariableType<Number>::operator()(const Number* it)
	{
		ITypePolicy* policy = IdentityManager::instance->GetTypeByOffset(NUMBER_SET_NAME, !it ? 0 : it->GetOffset());

		//Should already be specialized, so just sending it.
		return policy->FetchTypePolicy(nullptr);
	}


	AbstractTypePolicy* VariableType<String>::operator()(const String*)
	{
		ITypePolicy* policy = IdentityManager::instance->GetTypeByOffset(STRING_SET_NAME, CoreOffset::String);

		//Should already be specialized, so just sending it.
		return policy->FetchTypePolicy(nullptr);
	}


	AbstractTypePolicy* VariableType<Delegate>::operator()(const Delegate*)
	{
		return nullptr;
	}


	AbstractTypePolicy* VariableType<Object>::operator()(const Object* it)
	{
		//Currently there is no type policy, but at a later point yes.
		//it->GetType()

		return nullptr;
	}

	AbstractTypePolicy* VariableType<FunctionHandle>::operator()(const FunctionHandle*)
	{
		return nullptr;
	}


	AbstractTypePolicy* VariableType<Array>::operator()(const Array*)
	{
		return nullptr;
	}

	AbstractTypePolicy* VariableType<Variable>::operator()(const Variable* it)
	{
		if (it)
		{
			return it->Policy();
		}

		return nullptr;
	}

	AbstractTypePolicy* VariableType<double>::operator()()
	{
		//I could just make this numeric
		static AbstractTypePolicy* result = nullptr;

		if (!result) {

			//offset
			constexpr auto setting = LEX::Number::Settings::CreateFromType<double>();

			auto buffer = LEX::IdentityManager::instance->GetTypeByOffset("NUMBER", setting.GetOffset());

			result = buffer->FetchTypePolicy(nullptr);

			logger::info("id? {}", (int)result->FetchTypeID());
		}

		return result;
	}





	//This is so temporary I hate that I'm doing it like this.
	AbstractTypePolicy* Variable::_CheckVariableType()
	{
		AbstractTypePolicy* result = std::visit([&](auto&& lhs) {
			return FetchVariableType(lhs);
			}, _value);

		return result;
	}

	



	void test()
	{


	}




	class IFormula : public ICallableUnit
	{
		//IFormula simply should provide some clarification that this indeed is a formula

	};
	
	template <typename T>
	struct BasicFormula : public IFormula
	{
		//The idea of this is you cast an IFormula into this forcibly, and this type will then manage all of the function calls for the type.
		// Basically, think of this as a std::function. it should then translate all the rest of the bullshit around it.
	};


	class Formula : public IFormula, public BasicCallableData
	{
		//This version is obscured for the user. It should help inline functions and such into the code, or that can be given to others to run.
		//Formula rules. 
		// They don't have default parameters, 
		// they don't have procedures. 
		// They don't have names.
		// They don't have targets



		RuntimeVariable Invoke(std::vector<RuntimeVariable>& args, RuntimeVariable* def)
		{
			//TODO: Once arrays and the params keyword gets introduced, this will need to be implemented in other ways. Further more, could just bake this into the call.

			Runtime* runtime = nullptr;
			Variable* target = nullptr;

			if (args.size() != parameters.size())
				report::apply::critical("Arg size not compatible with param size ({}/{})", args.size(), parameters.size());

			size_t size = parameters.size();

			
			for (int i = 0; i < size; i++) {
				//Cancelling this for now.
				break;
				int j = i;

				AbstractTypePolicy* expected = parameters[i].GetType()->FetchTypePolicy(runtime);
				if (!expected)
					throw nullptr;
				RuntimeVariable check = args[j]->Convert(expected);

				if (check.IsVoid() == true)
					report::apply::critical("cannot convert argument into parameter {}, {} vs {}", parameters[i].GetFieldName(), i, j);

				args[i] = check;
			}
			
			{
				RuntimeVariable result;

				Runtime runtime{ *GetRoutine(), args };

				result = runtime.Run();

				return result;
			}
		}


	};


	namespace FunctionalInlining
	{
		

		//This will need a fair few things from the compiler in order to make this rigt. So I'm not dealing with it for right now.
		inline InstructType preserve = InstructType::Invalid;
		inline InstructType restore = InstructType::Division;

		static void ReserveRegister(RuntimeVariable&, Operand a_lhs, Operand a_rhs, InstructType instruct, Runtime* runtime)
		{
			auto mode = a_lhs.Get<Index>();

			Register exception = a_rhs.Get<Register>();

			//thinking about it, I may want to put these on the argument register. Mainly because the argument register is more likely to have less permenant increments

			Register reg = (Register)(Register::Total - 1);

			if (preserve) {
				size_t size = runtime->AdjustStackPointer(StackPointer::Argument, Register::Total);

				size_t it = size - Register::Total;

				for (; reg < Register::Total; reg--) {
					//Register::Total

					if (reg != exception)
						runtime->GetArgument(it + reg) = runtime->GetRegister(reg);
				}
			} else {
				size_t size = runtime->GetStackPointer(StackPointer::Argument);

				size_t it = size - Register::Total;

				//mode should equal it.

				//When this loops before 0 stack overflow will happen
				for (; reg < Register::Total; reg--) {
					//Register::Total

					if (reg != exception)
						runtime->GetRegister(reg) = runtime->GetArgument(it + reg);
				}
			}
		}

		inline void InlineRoutine(RoutineCompiler* compiler, RoutineBase& routine)
		{
			//compiler->ModParamCount(routine.argCapacity);
			//compiler->
			//routine.

			//The arguments should already be expected to go into variable slots rather than call sites by this point.
		}
	}

	//Calling function helper constructs. 
	//Needs names other than target. It's called a ThisVariable

	namespace detail
	{
		
		struct ThisHelperImpl
		{
			using _Base = ThisHelperImpl;

			//This is the thing that actually does the calling. This is given based on whether the target is a reference
			// type, or a pointer type.

			//The callable functions will have a few different versions, depending on one's faith in the system/preference.
			//For now, I'm not going to care about the alternate versions of this. BUT here is the low down on what I want
			//-A central and somewhat raw function for how I want everything packaged and sent to the Runtime
			//-A function that has doesn't the given references as variables
			//-A function that does use the given references as variables.
			//^ For this, one could just forward instead. Also, const should always be taken by value some how.




			//bool Invoke(Variable& out, std::vector<RuntimeVariable> args)

			Variable Call(ICallableUnit* call_unit, std::vector<Variable> args)
			{
				args.insert(args.begin(), _this);


				return {};
			}

			template <typename... Ts>
			Variable Call(ICallableUnit* unit, Ts&&... ts)
			{
				std::vector<Variable> args{};

				args.resize(sizeof...(Ts));

				return Call(unit, args);
			}


			//void* _this = nullptr;
			Variable _this;
		};
	}
	
	

	namespace NumberNew
	{
		using Settings = Number::Settings;


		enum struct NumberDataType
		{
			Invalid,
			Float,
			SInt,
			UInt,
		};


		union NumberData
		{

			size_t									_raw = 0;
			std::array<uint8_t, sizeof(size_t)>		_bytes;
			double									floating;
			int64_t									sInteger;
			uint64_t								uInteger;

			template<numeric T>
			auto& Get()
			{
				if constexpr (std::is_floating_point_v<T>) {
					return floating;
				} else if constexpr (std::is_same_v<T, uint64_t>) {
					return uInteger;
				} else {
					return sInteger;
				}
			}



			constexpr NumberData() = default;
			constexpr NumberData(double arg) : floating{ arg } {}//doesn't matter which it is.
			constexpr NumberData(int64_t arg) : sInteger{ arg } {}//doesn't matter which it is.
			constexpr NumberData(uint64_t arg) : uInteger{ arg } {}//doesn't matter which it is.
			constexpr NumberData(int32_t arg) : sInteger{ arg } {}
			constexpr NumberData(uint32_t arg) : sInteger{ arg } {}
			constexpr NumberData(int16_t arg) : sInteger{ arg } {}
			constexpr NumberData(uint16_t arg) : sInteger{ arg } {}
			constexpr NumberData(int8_t arg) : sInteger{ arg } {}
			constexpr NumberData(uint8_t arg) : sInteger{ arg } {}
			constexpr NumberData(bool arg) : sInteger{ arg } {}
		};
		REQUIRED_SIZE(NumberData, 0x8);


		struct NumberLimit
		{
			NumberData min;
			NumberData max;
			NumberData posInf;
			NumberData negInf;

			constexpr NumberLimit() = default;
			
			constexpr NumberLimit(NumberData _min, NumberData _max, std::optional<NumberData> pos_inf = std::nullopt, std::optional<NumberData> neg_inf = std::nullopt) :
				min {_min}, max {_max}, negInf{ neg_inf.value_or(0) }, posInf{pos_inf.value_or(0)}
			{

			}
		};

		//A few things, I want the access of this to be a function, second, this function should start an initializer.
		// This would allow me to make
		//Note, this isn't going to be accessed from one place, probably should be allowed to migrate be based on whatever system it's on.
		inline NumberLimit limitMap[NumeralType::Total][Signage::Total][Size::Total];
		

		void InitLimitMap()
		{
			static bool init = false;

			if (init)
				return;

			init = true;

			//For float, bit is episilon, byte is percent word. Nothing is used for word.
			

			//Note, these are given infinity REGARDLESS if it actually can use infinity or not
			// Also note, the value of infinity is a constant. Maybe also the only time that it will not be cleared.
			// An infinity of 0 means no infinity at all.
			limitMap[NumeralType::Floating][Signage::Signed][Size::Bit] = { -1.0, 1.0, INFINITY, -INFINITY };
			limitMap[NumeralType::Floating][Signage::Signed][Size::Byte] = { -100.0, 100.0, INFINITY, -INFINITY };
			limitMap[NumeralType::Floating][Signage::Signed][Size::Word] = { -100.0, 100.0, INFINITY, -INFINITY };
			limitMap[NumeralType::Floating][Signage::Signed][Size::DWord] = { min_value<float>, max_value<float>, INFINITY, -INFINITY };
			limitMap[NumeralType::Floating][Signage::Signed][Size::QWord] = { min_value<double>, max_value<double>, INFINITY, -INFINITY };

			limitMap[NumeralType::Floating][Signage::Unsigned][Size::Bit] = { 0.0, 1.0, 0, INFINITY };
			limitMap[NumeralType::Floating][Signage::Unsigned][Size::Byte] = { 0.0, 100.0, 0, INFINITY };
			limitMap[NumeralType::Floating][Signage::Unsigned][Size::Word] = { 0.0, 100.0, 0, INFINITY };
			limitMap[NumeralType::Floating][Signage::Unsigned][Size::DWord] = { 0.0, max_value<float>, 0, INFINITY };
			limitMap[NumeralType::Floating][Signage::Unsigned][Size::QWord] = { 0.0, max_value<double>, 0, INFINITY };



			limitMap[NumeralType::Integral][Signage::Unsigned][Size::Bit] = { (int64_t)false, (int64_t)true };
			limitMap[NumeralType::Integral][Signage::Unsigned][Size::Byte] = { min_value<uint8_t>, max_value<uint8_t>, max_value<uint64_t> };
			limitMap[NumeralType::Integral][Signage::Unsigned][Size::Word] = { min_value<uint16_t>, max_value<uint16_t>, max_value<uint64_t> };
			limitMap[NumeralType::Integral][Signage::Unsigned][Size::DWord] = { min_value<uint32_t>, max_value<uint32_t>, max_value<uint64_t> };
			limitMap[NumeralType::Integral][Signage::Unsigned][Size::QWord] = { min_value<uint64_t>, max_value<uint64_t>, max_value<uint64_t> };

			limitMap[NumeralType::Integral][Signage::Signed][Size::Bit] = { -1, 1, max_value<int64_t>, min_value<int64_t> };
			limitMap[NumeralType::Integral][Signage::Signed][Size::Byte] = { min_value<int8_t>, max_value<int8_t>, max_value<int64_t>, min_value<int64_t> };
			limitMap[NumeralType::Integral][Signage::Signed][Size::Word] = { min_value<int16_t>, max_value<int16_t>, max_value<int64_t>, min_value<int64_t> };
			limitMap[NumeralType::Integral][Signage::Signed][Size::DWord] = { min_value<int32_t>, max_value<int32_t>, max_value<int64_t>, min_value<int64_t> };
			limitMap[NumeralType::Integral][Signage::Signed][Size::QWord] = { min_value<int64_t>, max_value<int64_t>, max_value<int64_t>, min_value<int64_t> };

		}


		NumberLimit GetNumberLimit(NumeralType type, Signage sign, Size size)
		{
			InitLimitMap();
			return limitMap[type][sign][size];
		}



		

		INITIALIZE()
		{
			//there are 3 limit types


			InitLimitMap();
		}


		ENUM(InfiniteState, int8_t)
		{
			Negative = -1,
			Finite = 0,
			Positive = 1,
		};


		struct infinity
		{
			
			//This is a struct to represent infinity for whatever current setting is active. Helps represent infinity for types that don't actually have it.
		
			bool positive = true;
			NumeralType type{ NumeralType::Invalid };
			Size        size{ Size::Invalid };
			Signage     sign{ Signage::Invalid };

			//Also a note to this, I'd like to make constexpr versions of infinity to represent each version of infinity.


			infinity operator-() const
			{
				infinity result = *this;
				if (sign != Signage::Unsigned)
					result.positive = !result.positive;
				else
					report::message::break_warn("infinity cannot be negated with current settings");

				return result;
			}


			constexpr infinity() = default;

			constexpr infinity(NumeralType nt, Size sz, Signage sg) : type {nt}, size {sz}, sign {sg}
			{}


			//returns a constant for use when one wants to set a number to an infinite, but also doesn't want to 
			// override the settings.
			inline static constexpr infinity constant()
			{
				return infinity{};
			}

			inline static constexpr infinity float32()
			{
				return infinity{ NumeralType::Floating, Size::DWord, Signage::Signed };
			}

			inline static constexpr infinity float64()
			{
				return infinity{ NumeralType::Floating, Size::QWord, Signage::Signed };
			}

			inline static constexpr infinity ufloat32()
			{
				return infinity{ NumeralType::Floating, Size::DWord, Signage::Unsigned };
			}

			inline static constexpr infinity ufloat64()
			{
				return infinity{ NumeralType::Floating, Size::QWord, Signage::Unsigned };
			}




			//Also, positive will be restricted, instead, you must perform unary minus on infinity to get negative infinity.
		};


		namespace seperator
		{
			struct Number_
			{

				//Proving line.
			

				constexpr bool IsInfinite() const
				{
					return infinite != InfiniteState::Finite;
				}

				NumberDataType GetNumberType() const
				{
					if (_setting.IsFloat() == true)
					{
						return NumberDataType::Float;
					}
					else if (_setting.IsInteger() == true)
					{
						if (_setting.IsSigned() == true)
						{
							return NumberDataType::SInt;
						}
						else if (_setting.IsUnsigned() == true)
						{
							return NumberDataType::UInt;
						}
					}

					return NumberDataType::Invalid;
				}


				template <typename V>
				decltype(auto) Visit(V visitor)
				{
					//Visit will visit with a version of number data's different data types, or fail if the settings are invalid.

					switch (GetNumberType())
					{
					case NumberDataType::Float:
						return visitor(_data.floating);

					case NumberDataType::SInt:
						return visitor(_data.sInteger);

					case NumberDataType::UInt:
						return visitor(_data.uInteger);

					default:
						report::error("Invalid number settings. Data cannot be visited.");
						break;
					}
				}

				template <typename V>
				decltype(auto) Visit(V visitor) const
				{
					//Visit will visit with a version of number data's different data types, or fail if the settings are invalid.

					switch (GetNumberType())
					{
					case NumberDataType::Float:
						return visitor(_data.floating);

					case NumberDataType::SInt:
						return visitor(_data.sInteger);

					case NumberDataType::UInt:
						return visitor(_data.uInteger);

					default:
						report::error("Invalid number settings. Data cannot be visited.");
						break;
					}
				}




				Number_ Convert(Settings settings) const
				{
					//This is a really lazy and also incorrect way to do this.

					Number_ copy = settings;

					//copy._data = other._data;


					//The limit type observed on this is changed to Maximum(limits) if left is unsigned, and right is negative infinity.

					if (infinite && copy._setting.limit != Limit::Infinite)
					{
						auto _neg = GetNumberLimit(copy._setting.type, copy._setting.sign, copy._setting.size).negInf;
						auto _pos = GetNumberLimit(copy._setting.type, copy._setting.sign, copy._setting.size).posInf;



						switch (infinite)
						{
						case InfiniteState::Positive:
							copy._data = _pos;
							break;
						case InfiniteState::Negative:
							report::debug("neg");
							copy._data = _neg;
							break;
						}

						if (_setting.limit == Limit::Infinite) {
							copy.SetInfinite(infinite);
							return copy;
						}
					}

					auto& other = *this;

					copy.Visit([&]<typename L>(L& lhs)
					{
						other.Visit([&]<typename R>(R rhs)
						{
							lhs = (L)rhs;
						});

						copy.NewLimitCheck();
					});

					return copy;
				}

				Number_& Assign(const Number_& other)
				{
					*this = other.Convert(_setting);
					return *this;
				}
			

				std::string ToString() const
				{
					switch (infinite)
					{
					case InfiniteState::Finite:
						return Visit([](auto it) { return std::to_string(it); });
					case InfiniteState::Positive:
						return "inf";
					case InfiniteState::Negative:
						return "-inf";
					}


					return "INVALID";
				}



				//I'd like to change the name but this is good.
				Settings CompareSettings(const Number_& other, Settings ret)
				{
					constexpr Settings k_int{ NumeralType::Integral, Size::DWord, Signage::Signed, Limit::Overflow };
					constexpr auto k_offset = k_int.GetOffset();


					Settings result = _setting;

					if (auto settings = other._setting; result.GetOffset() < settings.GetOffset())
						result = settings;

					if (result.GetOffset() < k_offset)
						result = k_int;


					if (result.GetOffset() < ret.GetOffset())
						result = ret;

					return result;
					//other is the right hand side, "this" is the left hand side, and ret is the return of the predicate turned into a number setting.
					//The largest ont between these options, and int wins.
				}





				void SetInfinite(InfiniteState state)
				{
					//This is just so the same lambda isn't floating around.

					if (Visit([](auto it) { return it == 0; }) == false)
						infinite = state;
				}


				void NewLimitCheck(int overflow = 0)
				{
					auto _limit = GetNumberLimit(_setting.type, _setting.sign, _setting.size);

					auto _min = _limit.min;
					auto _max = _limit.max;

					if (!overflow)
					{
						overflow = Visit(
							[&]<typename T>(T value) -> int
							{
								return value > _max.Get<T>() ?
									1 : value < _min.Get<T>() ?
									-1 : 0;
							});


					}

					if (!overflow)
						return;


					switch (_setting.limit)
					{
						case Limit::Overflow:
						{
							Visit([&]<typename T>(T& value) 
							{
								using Cast = std::conditional_t<std::is_floating_point_v<T>, double, size_t>;
								value -= (_setting.IsSigned() + (size_t)(value / ((size_t)_max.Get<T>() + 1))) * ((size_t)_max.Get<T>() + 1);
							});
						}
						break;
						
						case Limit::Bound:
						{
							Visit([&](auto& value)
							{
								if (overflow == 1)
									_data = _max;
								else if (overflow == -1)
									_data = _min;
							});
						}
						break;

						case Limit::Infinite:
						{
							InfiniteState state = InfiniteState::Finite;

							if (overflow == 1) {
								_data = _limit.posInf;
								state = InfiniteState::Positive;
							}
							else if (overflow == -1) {
								_data = _limit.negInf;
								state = InfiniteState::Negative;
							}
							
							SetInfinite(state);							
						}
						break;
						
						default:
							report::message::error("Invalid limit.");
					}
				}



				template <typename Func>
				Number_ Operator(const Number_& other, Func func)
				{
					using Result = std::invoke_result_t<Func, bool, bool>;
					Number_ result = CompareSettings(other, Settings::CreateFromType<Result>());
					

					constexpr bool no_limit_check = requires(int lhs, int rhs)
					{
						func(lhs, rhs, 0);
					};

					double comp;


					result.Visit([&](auto& res)
					{
						this->Visit([&]<typename L>(L& lhs)
						{
							other.Visit([&]<typename R>(R rhs)
							{
								res = func(lhs, rhs);

								if constexpr (!no_limit_check)
									comp = func((double)lhs, (double)rhs);
							});
						});
					});



					//Should require an integral

					if constexpr (!no_limit_check)
					{
						int overflow = result.Visit([comp]<typename T>(T res) -> int
						{
							return  res > comp ?
								-1 : res < comp ?
								1 : 0;
						});


						//report::trace("overflow {}", overflow);
						//bool bound = false;

						result.NewLimitCheck(overflow);
					}

					return result;
				}


				//-end
			


			
				Number_() = default;
				
				Number_(Settings _set) :
					// _value{ v },
					_setting{ _set },
					_priority{ _setting.GetOffset() }
				{
					const char* name = "INVALID";

					Visit([&]<typename T>(T) { name = typeid(T).name(); });


					RGL_LOG(trace, "Number<{}> ctor {}, sets: sign {} size {} num {} lim {}", name,
						ToString(),
						_setting.sign, _setting.size, _setting.type, _setting.limit);
				}

				template <numeric T>
				Number_(T v) :
					_setting{ Settings::CreateFromType<T>() },
					_priority{ _setting.GetOffset() }
				{
				
					_data.Get<T>() = v;
				

					RGL_LOG(trace, "Number<{}> ctor {}, sets: sign {} size {} num {} lim {}", typeid(T).name(),
						ToString(),
						_setting.sign, _setting.size, _setting.type, _setting.limit);
				}

				Number_(infinity inf)
				{

					if (inf.type == NumeralType::Invalid || inf.sign == Signage::Invalid || inf.size == Size::Invalid) {
						report::fault::break_error("Number given an infinite with invalid settings. Type: {}, Sign: {}, Size: {}", (int)inf.type, (int)inf.sign, (int)inf.size);
					}

					_setting.limit = Limit::Infinite;

					*this = inf;

					RGL_LOG(trace, "Number<infinity?> ctor {}, sets: sign {} size {} num {} lim {}",
						ToString(),
						_setting.sign, _setting.size, _setting.type, _setting.limit);
				}

				Number_& operator= (infinity inf)
				{
					//Infinity will completely ignore it's own settings if they're all
					auto type = inf.type != NumeralType::Invalid ? inf.type : _setting.type;
					auto sign = inf.sign != Signage::Invalid ? inf.sign : _setting.sign;
					auto size = inf.size != Size::Invalid ? inf.size : _setting.size;

					_setting.type = type;
					_setting.sign = sign;
					_setting.size = size;

					//You can't set things to infinity (yet) because it doesn't have settings. Once I make it have some settings,
					// just the non-limit ones I'll make it handle that.
				
					if (inf.positive){
						_data = GetNumberLimit(type, sign, size).posInf;
					}
					else{
						_data = GetNumberLimit(type, sign, size).negInf;
					}

					if (_data._raw != 0 && _setting.limit == Limit::Infinite)
						infinite = inf.positive ? InfiniteState::Positive : InfiniteState::Negative;

					_priority = _setting.GetOffset();

					return *this;
				}

			
				template <numeric T>
				T Get() const
				{
					return Visit([](auto it)->T {return static_cast<T>(it); });
				}

				template <numeric T>
				bool As(T& value) const
				{
					constexpr NumberDataType type = std::is_floating_point_v<T> ?
						NumberDataType::Float : std::is_unsigned_v<T> ?
						NumberDataType::UInt : NumberDataType::SInt;

					if (GetNumberType() != type)
						return false;

					Visit([](auto it){return value = static_cast<T>(it); });

					return true;
				}

				
				NumberData _data;
				Settings _setting;
				uint8_t _priority;//Space is free so I might as well
				InfiniteState infinite = InfiniteState::Finite;//If active, it acts as infinity. If it's tried to transfer into
			};





			INITIALIZE()
			{
#undef max
#undef min
				uint8_t r = 0;
				bool tests[60]{};
				for (NumeralType a = (NumeralType)0; a < NumeralType::Total; a++)
				{
					for (Signage c = (Signage)0; c < Signage::Total; c++)
					{
						for (Size b = (Size)0; b < Size::Total; b++)
						{
							for (Limit d = (Limit)0; d < Limit::Total; d++)
							{
								report::info("offset test {}-{}", GetNumberOffset(a, b, c, d), ++r);
								
							}
							
						}
					}
					logger::debug("end");
				}

				report::info("final {}", r);

				logger::critical("starting process now");

				Number_ testA = std::numeric_limits<int32_t>::max();
				Number_ testB = 266;


				auto lambda = [](auto lhs, auto rhs) { auto res = lhs * rhs; return res; };

				//auto data = testB.OperateData(4, [](auto lhs, auto rhs) { auto res = lhs * rhs; return res; });
				//report::debug("data is {}", data.sInteger);


				Number_ testResult = testA.Operator(testB, lambda);

				Number_ unsigned_int8 = (uint8_t)1;

				Number_ test_inf = -infinity::ufloat64();


				report::debug("inf test {}", test_inf.ToString());

				unsigned_int8.Assign(testB);
				report::debug("uint8 {}", unsigned_int8.ToString());

				report::debug("{} * {} = {} ?", testA.ToString(), testB.ToString(), testResult.ToString());
				constexpr auto fill = std::numeric_limits<int8_t>::max();

				unsigned_int8.Visit([](auto& it) { it += 1; });

				report::debug("uint8 later {}", unsigned_int8.ToString());
			}

		}


		struct Number_
		{
			NumberData _data;
			Settings _setting;
			uint8_t _priority;//Space is free so I might as well
			InfiniteState infinite = InfiniteState::Finite;//If active, it acts as infinity. If it's tried to transfer into
			Number_() = default;
			Number_(Settings _set) :
				// _value{ v },
				_setting{ _set },
				_priority{ _setting.GetOffset() }
			{
				const char* name = "INVALID";

				if (_setting.IsFloat() == true)
				{
					name = typeid(double).name();
				}
				else if (_setting.IsInteger() == true)
				{
					if (_setting.IsSigned() == true)
					{
						name = typeid(int64_t).name();
					}
					else if (_setting.IsUnsigned() == true)
					{
						name = typeid(uint64_t).name();
					}
				}


				RGL_LOG(trace, "Number<{}> ctor {}, sets: sign {} size {} num {} lim {}", name,
					ToString(),
					_setting.sign, _setting.size, _setting.type, _setting.limit);
			}
			template <numeric T>// requires (!std::is_integral_v<T>)
			Number_(T v) :
				// _value{ v },
				_setting{ Settings::CreateFromType<T>() },
				_priority{ _setting.GetOffset() }
			{

				_data.Get<T>() = v;


				RGL_LOG(trace, "Number<{}> ctor {}, sets: sign {} size {} num {} lim {}", typeid(T).name(),
					ToString(),
					_setting.sign, _setting.size, _setting.type, _setting.limit);
			}

			Number_& operator= (infinity inf)
			{
				//You can't set things to infinity (yet) because it doesn't have settings. Once I make it have some settings,
				// just the non-limit ones I'll make it handle that.
				auto _neg = GetNumberLimit(_setting.type, _setting.sign, _setting.size).negInf;
				auto _pos = GetNumberLimit(_setting.type, _setting.sign, _setting.size).posInf;


				if (inf.positive)
				{
					_data = GetNumberLimit(_setting.type, _setting.sign, _setting.size).posInf;
				}
				else
				{
					_data = GetNumberLimit(_setting.type, _setting.sign, _setting.size).negInf;
				}

				if (OperateData(0, [](auto l, auto r) {return l == r; }) == false)
					infinite = inf.positive ? InfiniteState::Positive : InfiniteState::Negative;

				return *this;
			}


			template <numeric T>
			auto GetValue()
			{
				auto raw = _data.Get<T>();

				//if (infinity)
				// return ...
			}

			template <numeric L, numeric R>
			void SecondStep_convert(L& lhs, R rhs)
			{
				lhs = (L)rhs;
			}

			template <numeric L>
			void FirstStep_convert(L& lhs, const Number_& other)
			{
				//By this point, "this" should be a number that is created based on the settings between 2 types.
				//auto return_type = CreateFromType

				switch (other._setting.type) {
				case NumeralType::Floating:
					SecondStep_convert(lhs, other._data.floating);
					break;

				case NumeralType::Integral:
					if (_setting.sign == Signage::Signed)
						SecondStep_convert(lhs, other._data.sInteger);
					else if (_setting.sign == Signage::Unsigned)
						SecondStep_convert(lhs, other._data.uInteger);
					else
						report::critical("a failure noted.");
					break;
				}
			}



			template<numeric T, typename Pred> requires(std::is_same_v<std::invoke_result_t<Pred, int, int>, bool>)
				auto OperateData(T number, Pred predicate)
			{
				//pred has to be able to take 2 entries
				switch (GetStoredNumeral())
				{
				case 1:
					return predicate(_data.floating, number);
				case 2:
					return predicate(_data.sInteger, number);
				case 3:
					return predicate(_data.uInteger, number);
				}

				report::error("invalid number type settings are invalid.");
			}

			template<numeric T, typename Pred>
			NumberData OperateData(T number, Pred predicate)
			{
				//pred has to be able to take 2 entries
				switch (GetStoredNumeral())
				{
				case 1:
					return predicate(_data.floating, number);
				case 2:
					return predicate(_data.sInteger, number);
				case 3:
					return predicate(_data.uInteger, number);
				}

				report::error("invalid number type settings are invalid.");
			}

			Number_ Convert(const Number_& other)
			{
				//This is a really lazy and also incorrect way to do this.

				Number_ copy = _setting;

				//copy._data = other._data;


				//The limit type observed on this is changed to Maximum(limits) if left is unsigned, and right is negative infinity.

				if (other.infinite && copy._setting.limit != Limit::Infinite)
				{
					auto _neg = GetNumberLimit(copy._setting.type, copy._setting.sign, copy._setting.size).negInf;
					auto _pos = GetNumberLimit(copy._setting.type, copy._setting.sign, copy._setting.size).posInf;



					switch (other.infinite)
					{
					case InfiniteState::Positive:
						copy._data = _pos;
						break;
					case InfiniteState::Negative:
						report::debug("neg");
						copy._data = _neg;
						break;
					}

					if (_setting.limit == Limit::Infinite) {
						if (copy.OperateData(0, [](auto l, auto r) {report::debug("{}", l); return l == r; }) == false)
							copy.infinite = other.infinite;

						return copy;
					}
				}

				//This can be simplified.
				switch (GetStoredNumeral())
				{
				case 1:
					copy.FirstStep_convert(copy._data.floating, other);
					copy.CheckBounds<double>();
					break;

				case 2:
					copy.FirstStep_convert(copy._data.sInteger, other);
					copy.CheckBounds<int64_t>();
					break;

				case 3:
					copy.FirstStep_convert(copy._data.uInteger, other);
					copy.CheckBounds<uint64_t>();
					break;
				}

				return copy;

			}


			std::string ToString() const
			{
				switch (infinite)
				{
				case InfiniteState::Finite:
					switch (GetStoredNumeral())
					{
					case 1:
						return std::to_string(_data.floating);


					case 2:
						return std::to_string(_data.sInteger);

					case 3:
						return std::to_string(_data.uInteger);

					default:
						break;
					}

				case InfiniteState::Positive:
					return "inf";
				case InfiniteState::Negative:
					return "-inf";
				}


				return "INVALID";
			}

			Settings DuelTypes(const Number_& other, Settings ret)
			{
				constexpr Settings k_int{ NumeralType::Integral, Size::DWord, Signage::Signed, Limit::Overflow };
				constexpr auto k_offset = k_int.GetOffset();


				Settings result = _setting;

				if (auto settings = other._setting; result.GetOffset() < settings.GetOffset())
					result = settings;

				if (result.GetOffset() < k_offset)
					result = k_int;


				if (result.GetOffset() < ret.GetOffset())
					result = ret;

				return result;
				//other is the right hand side, "this" is the left hand side, and ret is the return of the predicate turned into a number setting.
				//The largest ont between these options, and int wins.
			}


			//make some enum later.
			int GetStoredNumeral() const
			{
				if (_setting.IsFloat() == true)
				{
					return 1;
				}
				else if (_setting.IsInteger() == true)
				{
					if (_setting.IsSigned() == true)
					{
						return 2;
					}
					else if (_setting.IsUnsigned() == true)
					{
						return 3;
					}
				}

				return 0;
			}

			NumberDataType GetNumberType() const
			{
				if (_setting.IsFloat() == true)
				{
					return NumberDataType::Float;
				}
				else if (_setting.IsInteger() == true)
				{
					if (_setting.IsSigned() == true)
					{
						return NumberDataType::SInt;
					}
					else if (_setting.IsUnsigned() == true)
					{
						return NumberDataType::UInt;
					}
				}

				return NumberDataType::Invalid;
			}


			template <typename V>
			auto Visit(V visitor)
			{
				//Visit will visit with a version of number data's different data types, or fail if the settings are invalid.

				switch (GetNumberType())
				{
				case NumberDataType::Float:
					logger::info("a");
					return visitor(_data.floating);


				case NumberDataType::SInt:
					logger::info("b");
					return visitor(_data.sInteger);

				case NumberDataType::UInt:
					logger::info("c");
					return visitor(_data.uInteger);

				default:
					report::error("Invalid number settings. Data cannot be visited.");
					break;
				}
			}

			int GetInfinity()
			{
				switch (GetStoredNumeral())
				{
				case 1:
				{
					auto value = _data.Get<double>();

					if (std::numeric_limits<double>::infinity() == value)
						return 1;
					if (-std::numeric_limits<double>::infinity() == value)
						return -1;
					break;
				}

				}

				return 0;

			}

			template <numeric T>
			void HandleBounds(int overflow, T _min, T _max)
			{
				//bounds only cares if the current value is outside of the given bounds of its own type.

				if (GetInfinity() != 0)
					return;


				//auto value = _data.Get<T>();

				if (overflow == 1)
					_data.Get<T>() = _max;
				if (overflow == -1)
					_data.Get<T>() = _min;
			}

			template<numeric T>
			void CheckBounds()
			{
				auto _min = GetNumberLimit(_setting.type, _setting.sign, _setting.size).min.Get<T>();
				auto _max = GetNumberLimit(_setting.type, _setting.sign, _setting.size).max.Get<T>();

				auto& value = _data.Get<T>();

				if (_setting.limit == Limit::Overflow)
				{
					value -= (_setting.IsSigned() + (size_t)(value / ((size_t)_max + 1))) * ((size_t)_max + 1);
					return;
				}



				//Possibly make overflow a parameter?
				int overflow = value > _max ?
					1 : value < _min ?
					-1 : 0;

				return HandleBounds(overflow, _min, _max);
			}

			template <typename Pred, numeric L, numeric R>
			void SecondStep(L lhs, R rhs, Pred predicate)
			{
				using Result = std::invoke_result_t<Pred, L, R>;

				//I have this idea that there's a possible third argument the predicate can have, this will allow me to tell if I care about integer overflow or not.
				//Here might be the settings
				//care
				//care but only if integer
				//care but only if can be transfered into double. This might just be care.
				//doubles are basically what I use for guards. They aren't very accurate, but I assume they'll be able to handle the differences between
				// good enough values and maxes.

				auto result = predicate(lhs, rhs);

				_data.Get<Result>() = predicate(lhs, rhs);

				if constexpr (std::is_floating_point_v<Result>) {
					_data.floating = predicate(lhs, rhs);
				}
				else if constexpr (std::is_same_v<Result, uint64_t>) {
					_data.sInteger = predicate(lhs, rhs);
				}
				else {
					_data.sInteger = predicate(lhs, rhs);
				}

				constexpr bool no_overflow = !std::is_floating_point_v<Result> && requires(L lhs, R rhs)
				{
					predicate(lhs, rhs, 0);
				};

				//Should require an integral

				if constexpr (!no_overflow)
				{

					auto _min = GetNumberLimit(_setting.type, _setting.sign, _setting.size).min.Get<Result>();
					auto _max = GetNumberLimit(_setting.type, _setting.sign, _setting.size).max.Get<Result>();


					int overflow = result > _max ?
						1 : result < _min ?
						-1 : 0;


					//This is an overflow safety check in case the value is too large, but can be missed if it's one of the lesser conversions.
					if (!overflow)
					{
						auto comp = predicate(lhs, rhs);


						//This version of overflow only happens if the size of the result given types is too large to actually handle properly.
						overflow = result > comp ?
							-1 : result < comp ?
							1 : 0;
					}

					report::trace("overflow {}", overflow);
					//bool bound = false;

					//This has to be made into a function.
					if (false)
					{
						//this only happens if it caps itself specifically.
						if (overflow == 1)
							_data.Get<Result>() = _max;
						if (overflow == -1)
							_data.Get<Result>() = _min;
					}
					else
					{
						auto& data = _data.Get<Result>();


						data -= (_setting.IsSigned() + (size_t)(data / ((size_t)_max + 1))) * ((size_t)_max + 1);;

					}

					//An interesting phenomenon is if you use the keywords and parentheses, you can make an expression handle it's given types
					// as if it were another limit type.
				}
			}


			template <typename Pred, numeric L>
			void FirstStep(L lhs, const Number_& other, Pred predicate)
			{
				//By this point, "this" should be a number that is created based on the settings between 2 types.
				//auto return_type = CreateFromType

				switch (other._setting.type) {
				case NumeralType::Floating:
					SecondStep(lhs, other._data.floating, predicate);
					break;

				case NumeralType::Integral:
					if (_setting.sign == Signage::Signed)
						SecondStep(lhs, other._data.sInteger, predicate);
					else if (_setting.sign == Signage::Unsigned)
						SecondStep(lhs, other._data.uInteger, predicate);
					else
						report::critical("a failure noted.");
					break;
				}
			}

			template <typename Pred>
			Number_ Operator(const Number_& other, Pred predicate)
			{
				//The only way this actually matters if it's a boolean. The question is, how to test this fact?
				// We given it the lowest priority version that it could be. A boolean. so pre

				using PredResult = std::invoke_result_t<Pred, bool, bool>;
				Number_ result = DuelTypes(other, Settings::CreateFromType<PredResult>());

				switch (_setting.type)
				{
				case NumeralType::Floating:
					result.FirstStep(_data.floating, other, predicate);
					break;

				case NumeralType::Integral:
					if (_setting.sign == Signage::Signed)
						result.FirstStep(_data.sInteger, other, predicate);
					else if (_setting.sign == Signage::Unsigned)
						result.FirstStep(_data.uInteger, other, predicate);
					else
						report::critical("a failure noted.");
					break;
				}

				return result;

			}

		};


		template <typename Pred>
		void TestTemplate(Pred predicate)
		{
			predicate(1, 2);
			predicate(1.0, 2.0);
			predicate(1.0f, 2.0f);
			predicate(true, 2.0f);
			
		}
		

		INITIALIZE()
		{
#undef max
#undef min
			
			return;

			report::info("starting process now");

			Number_ testA = std::numeric_limits<uint32_t>::max();
			Number_ testB = 266;
			

			auto lambda = [](auto lhs, auto rhs) { auto res = lhs * rhs; report::info("res = {}", res); return res; };

			auto data =  testB.OperateData(4, [](auto lhs, auto rhs) { auto res = lhs * rhs; return res; });
			report::debug("data is {}", data.sInteger);


			Number_ testResult = testA.Operator(testB, lambda);

			Number_ unsigned_int8 = (uint8_t)1;
			
			Number_ test_inf = 1.0;

			test_inf = infinity{};
			
			report::debug("inf test {}", test_inf.ToString());
			report::debug("inf test {}", test_inf.ToString());

			unsigned_int8 = unsigned_int8.Convert(testB);
			report::debug("uint8 {}", unsigned_int8.ToString());

			report::debug("{} * {} = {} ?", testA.ToString(), testB.ToString(), testResult.ToString());
			constexpr auto fill = std::numeric_limits<int8_t>::max();

			unsigned_int8.Visit([](auto& it) { it += 1; });

			report::debug("uint8 later {}", unsigned_int8.ToString());
			return;
			
			TestTemplate([](auto a1, auto a2) {});

			//This is my simple way to tell if stack overflow has happened.
			constexpr auto a = std::numeric_limits<int64_t>::max();
			constexpr auto b = std::numeric_limits<int32_t>::min();

			#define OP +

			constexpr auto aish = (a OP b);
			constexpr auto bish = ((double)a OP (double)b);

			constexpr auto test = aish == bish;

			
			constexpr int weight = aish > bish ?
				-1 : aish < bish ?
				1 : 0;
		
			constexpr auto modolo = a % b;
			Number_ test_num{};
			test_num.SecondStep(1, 1, [](auto, auto) { return 1; });
		}

	}


	//*

	//std::convertible_to<Variable>//I want to use this, but I cannot because it doesn't yet account for pointer
	// types. Readdress another time.
	template <typename T, typename = void>
	struct ThisHelper {};



	//This is for value and references
	template <typename T>
	struct ThisHelper<T, std::enable_if_t<!PointerType<T>>> : public detail::ThisHelperImpl
	{
		using _Self = ThisHelper<T>;

		//Should be convertible to a variable?

		//This is only to fulfill what someone would normally use


		ThisHelper(const T& t)
		{

		}

		ThisHelper(const T&& t)
		{

		}
	};

	//this is for pointers
	template <typename T>
	struct ThisHelper<T, std::enable_if_t<PointerType<T>>> : private detail::ThisHelperImpl
	{
		using TypeC = int;
		using _Self = ThisHelper<T>;

		_Base* operator->()
		{
			//This gets the TargetBase, the thing that actually has the run function, to resume the 
			// syntax that you're accessing a member function (which you kinda are).

			return this;
		}


		ThisHelper(const T& t)
		{

		}

		ThisHelper(const T&& t)
		{

		}
	};


	template <typename T>
	ThisHelper(T) -> ThisHelper<T>;



	//this helper should be moved to something else.
//maybe name METHOD?
#define METHOD(mc_tar) ThisHelper{ mc_tar }



	void test_method()
	{
		std::string* string_thing;
		std::vector testname{ string_thing };

		METHOD(string_thing)->Call(nullptr, 1, 3, 4);

	}
#undef METHOD
	//*/




	//Creator function for numbers

	inline void TestCreateAllSettings()
	{
		return;
		//This is the name of the unique policy for numbers, that can handle the conversions between things itself.
		using NumericPolicy = ConcretePolicy;

		std::vector<ConcretePolicy*> results;

		NumeralType type{};
		Size        size{};
		Signage     sign{};
		Limit       limit{ Limit::Overflow };

		IdentityManager::instance->GenerateID("NUMBER", Number::Settings::length);

		ConcretePolicy* primary_policy = new ConcretePolicy{ "NUMBER", 0 };

		for (int a = 0; a < NumeralType::Total; a++)
		{
			for (int b = 0; b < Size::Total; b++)
			{
				for (int c = 0; c < Signage::Total; c++)
				{
					for (int d = 1; a < Limit::Total; d++)
					{
						Number::Settings settings{ (NumeralType)a, (Size)b, (Signage)c, (Limit)d };
						ConcretePolicy* number_policy = new ConcretePolicy{ "NUMBER", settings.GetOffset() };
						Variable defaultValue{ settings, number_policy };
						number_policy->EmplaceDefault(defaultValue);
						results.emplace_back(number_policy);


					}
				}
			}
		}

		//static TypePolicy* float64 = new TypePolicy{ "NUMBER", Number::Settings::GetOffset(NumeralType::Floating) };

		//do something with values.
	}

	
	class IntrinsicPolicy : public ConcretePolicy
	{
		//The concept of an intrinsic policy is first simply that intrinsic policies are the only 
		// policies that can Claim a specific space. Think like how void would or something like that.
			
		//Second, further branches of it will be able to present default versions of things such as strings, numbers
		// arrays, etc. This prevents us from having to carry the variable around in every single type despite not needing it.

		//There will likely have to be one for generic, but every generic/concrete one won't need new satelites, as they
		// will just assume that it is a concrete policy. Which it is.





	};











	struct TypeA1 {};
	struct TypeA2 :public TypeA1 {};

	struct TypeB1 {};
	struct TypeB2 : public TypeB1 {};

	struct TypeA : public TypeA2 {};
	struct TypeB : public TypeA, public TypeB2 {};
	struct TypeC : public TypeB {};
	//TypeC hash to TypeB2
	//i:0, 1
	//TypeC hash to TypeB1
	//i:0, 1, 0
	//TypeC hash to TypeA1
	//i:0, 0, 0, 0

	//So here's a thing of note, as a hash 0 hash to be non-modifiying.
	
	namespace NewSpecials
	{

		//Things and structures I need to replicate to continue vs ones I do not need to relicate.
		struct BasicGeneric;


		//Dont
		class Specialization
		{
			//virtual ~Specialization() {}


			BasicGeneric* _parent = nullptr;
		};

		//Dont
		//These are both bases to a template class that handles it. I think.
		class ResolvedVariant : public Specialization
		{
			//this is abstract


			std::vector<AbstractTypePolicy*> _types;
		};

		//Dont
		class UnresolvedVariant : public Specialization
		{
			//this is interface

			//I dunno what to call this, but basically it's an "AdjustPointer" to remove the need
			// of a dynamic cast. Unsure if I'll do this though, probably will not. A dynamic cast is not expensive
			// after all.
			void* AsSpecial() { return nullptr; }

			std::vector<ITypePolicy*> _types;


		};


		//Dont
		struct BasicGeneric
		{

		};

		//Dont
		template <typename T1, typename T2>
		struct Generic : public BasicGeneric, public virtual T1
		{


			using Interface = T1;		//The interface is the type used to symbolize the idea of being the type.
			using Abstract = T2;		//The abstraction is the type used to say something is ready to be used as the type.

			struct ResolveInterface : public Interface, public ResolvedVariant
			{

			};

			//The existence of this is not so that the interface can become the abstract, rather it's so this
			// has a way of communicating with it's later version on how to do that very thing.
			virtual Abstract* ResolveVariant(Interface*, IGenericArgument*) = 0;

			//So the idea I think I'll have is instead you'll be forced to make a resolve interface and resolve

			//size_t _expectedArgs = 0;					//args
			//std::vector<GenericParamPolicy> _params;	//params
			//std::vector<UnresolvedVariant*> _clients;	//partial
			//std::vector<ResolvedVariant*> _servers;		//complete
		};


		//Do
		struct Interface
		{
			virtual ~Interface() = default;
		};

		//Do
		struct Abstract : public virtual Interface
		{

		};



		//Do
		//Advisable base structure that should be the pivot between generic and concrete classes.
		struct Base : public virtual Interface//, FunctionData, ICallableUnit
		{

		};


		//Dont
		//This is what a generic class would look like.
		struct GenericFoo : public Base, public Generic<Interface, Abstract>
		{
			//This is generic
			//void a() override {}
			//int c;

			Abstract* ResolveVariant(Interface* tar, IGenericArgument*)
			{
				return nullptr;
			}
		};

		//Do
		struct ConcreteFoo : public Base, public Abstract
		{
			//This is concrete.
			//int d;
		};
		//The above structure seems to work, most importantly, it doesn't take undue size for the concrete type.
		// At least, not much.


		int main()
		{
			ConcreteFoo objA{};
			GenericFoo objB{};

			Abstract* test1 = &objA;
			Interface* test2 = &objB;

			//objA.a();
			//objB.a();
			std::cout << "Hello World << " << sizeof(ConcreteFoo);

			return 0;
		}

	}

	struct T
	{

		int foo(TypeA2&)
		{
			return 1;
		}

		int foo(const TypeA2&)
		{
			return 1;
		}



		int foo(TypeB2&)
		{
			return 1;
		}



		int bar(TypeA1&)
		{
			return 1;
		}


		int bar(TypeB1&, TypeB1&)
		{
			return 1;
		}
		

		int bar(const TypeC&, TypeB1&)
		{
			return 1;
		}

		struct IBase2 {
			static void Foo() {};

			virtual void Bar()
			{

			}
		};

		struct IBase :public virtual IBase2
		{
			void Bar2() const {}
			void Bar() override{}
		};

		struct DiffBase : public virtual IBase2 { static void Foo() {}; };


		struct BaseA : public virtual IBase { void Bar() override {} };
		struct BaseB : public virtual IBase {};

		struct Derived : public BaseA, public BaseB {};
		struct Derived2 :public Derived, public DiffBase { void Bar2() {} };




		int pie(const IBase&, float, int)
		{
			return 1;
		}

		int pie(const IBase2&, int, float)
		{
			return 1;
		}

		//int pie(const Derived&){return 1;}


		void Test()
		{

			TypeA a;
			TypeB b;
			TypeC c;
			foo(a);
			//First: arg1 par 3, arg2 par 3 (or actual 5 if we consider multiple inherit)
			//Second call: arg1 par 7, arg2 par 4

			//foo(c);

			bar(c, c);

			Derived2 d;
			const Derived2 d2;
			d.Bar();
			//d2.Bar2();

			Derived2::Foo();
			//pie(d, 1, 1);
		}
	};









	struct Array_
	{
		int8_t size = 0;
		std::vector<Variable> stuff;

		Array_()
		{
			logger::info("Creating Array");
		}

		~Array_()
		{
			logger::info("Deleting Array");
		}
	};

	template <>
	struct LEX::ObjectInfo<Array_> : public QualifiedObjectInfo<Array_>
	{
		using Typer = int;

		bool IsPooled(ObjectData&) override
		{
			return true;
		}

		TypeOffset GetTypeOffset(ObjectData&) override
		{
			return 0;
		}
	};


	template <>
	decltype(auto) ToObject<std::vector<int>>(std::vector<int>& obj)
	{
		logger::info("hitting");
		Array_ result{};

		//const std::vector<void*> test;

		//void* other = test[1];


		std::transform(obj.begin(), obj.end(), std::back_inserter(result.stuff), [&](auto it) {return it; });

		result.size = obj.size();
		return result;
	}




	
	INITIALIZE()
	{
		RegisterObjectType<Array_>("ARRAY");
		
		return;
		{
			Object _array1 = GetObjectPolicy<Array_>()->CreateObject(0);
			Object _array2 = GetObjectPolicy<Array_>()->CreateObject(0);

			_array1 = _array2;
		}
		{
			Object _array1 = GetObjectPolicy<Array_>()->CreateObject(0);
			Object _array2 = GetObjectPolicy<Array_>()->CreateObject(0);

			_array1 = _array2;
		}


		return;
		// To walk through functionality, how would this work ?
		std::vector<int> vars{ 1, 2, 3, 4, 5, 6, 7 };

		//auto* var_ptr = &vars;

		//Object test_array = MakeObject(var_ptr);


		Object _array = MakeObject(vars);
		bool policy = _array.policy;
		//_array.data.fastValue
		//logger::critical("Object check: {} , {} (+info {})", reinterpret_cast<Array_*>(_array.data.ptrVal)->size, policy, policy ? !!_array.policy->base : false);
		logger::critical("Object check: {} , {} (+info {})", _array.get<Array_>().size, policy, policy ? !!_array.policy->base : false);


		std::string str;

		//Object str_obj = MakeObject(str);





		//for (auto& variable : reinterpret_cast<Array_*>(_array.data.ptrVal)->stuff)
		for (auto& variable : _array.get<Array_>().stuff)
		{
			logger::critical("var: {}", variable.AsNumber());
		}



		//Array_ new_array = *reinterpret_cast<Array_*>(_array.data.ptrVal);
		Array_ new_array = _array.get<Array_>();

		for (auto& variable : new_array.stuff)
		{
			logger::critical(" new var: {}", variable.AsNumber());
		}


		//logger::critical("GenericBuilt check: {} ", reinterpret_cast<Array_*>(GenericDataBuilder<Array_>().ptrVal)->size);
		logger::critical("GenericBuilt check: {} ", GenericDataBuilder<Array_>().get<Array_>().size);

		//return;
		//static_assert(false, "The object pooling system isn't doing so hot. Confirm that it's not stupid busted later.");

		Object first = MakeObject(vars);

		logger::info("index {}", first.data.idxVal);
		Object second = first;

		first = second;

		second.Destroy();


		first = second;
		logger::info("end of test");
	};



#define EXTEND(mc_object) 1

template <typename T>
struct Extension : public T
{
	//the idea is this is a struct that either takes a reference derives from it with no vtable and attempts to perform some new function on it.

	//The only problem is making additional increases to the extension methods.
};


#define TO_XVALUE(mc_expr) true ? mc_expr : static_cast<decltype(mc_expr)&&>(*(int*)0)

	namespace Fields
	{


		//Unsure if this should be using a reference to the runtime variable. I guess not? This doesn't intend to
		// actually alter the register, just the value the register holds.
		using __Func = RuntimeVariable(*)(RuntimeVariable);
		

		template<__Func Func>
		struct Convert final : public ICallableUnit
		{
			using _Self = Convert<Func>;

			static _Self& GetSingleton()
			{
				static _Self singleton{};

				return singleton;
			}


			inline static _Self* instance = &GetSingleton();

			virtual RuntimeVariable Invoke(std::vector<RuntimeVariable>& args, RuntimeVariable* def)
			{

				//Convert should be real simple.
				//here, it's supposed to return the conversion.
				return Func(args[0]);
			}


			Convert(__Func func) {}

		protected:
			constexpr Convert() = default;
		
			Convert(const Convert&) = delete;
			Convert(Convert&&) = delete;
			Convert& operator=(const Convert&) = delete;
			Convert& operator=(Convert&&) = delete;

		};



		auto converting = Convert<nullptr>::instance;


		template <typename T>
		struct Derived : public T, public Void
		{
			//How do we handle T?
			//First


			void Foo(int i)
			{
				//We cannot do this, because we don't know from these unresolved types if these are compatible.
				i = *this;
			}


			//What we learned.
			//-First off, that the inheritence list will be completely different between concrete and generic types.
			//  From a generic standpoint, it inherits from all the things it knows it can inherit. More than likely,
			//  it will inherit from these FIRST, regardless order. The main reason why is if I try to unpack them in
			//  the order given, what if something that we inherit from later is inherited from sooner?
			// -Such as classC derives from Interface, so it's member offset is like 4, but our generic type 
			//  also derives from it, making it a lot sooner, making the member offset inaccurate between specializations.
			//  This is still problematic however. Mainly because of member access to non-existent parts.
			// <!>Ah, I know what will resolve this. if the generic has typed constraints that allow it to derive from concrete types, it will use those concrete types
			//  within the generic class. Note, this will also result in those being sooner than thought.

			//The question operating with this is how to handle the adjustments when calling functions that expect something different?


			//Inherit list
			//Concrete:
			// Void

			//Specialization
			//T...

			//Total


		};

		void TestConvert()

		{
			auto fill = std::vector{ 1 };


			constexpr auto lambda = [](RuntimeVariable it)->RuntimeVariable {return it; };
			auto val1 = Convert <lambda> ::instance;
		}
	}


	namespace detail
	{
		template<typename T, size_t Size>requires(sizeof(size_t) >= Size)
		struct alignas(bool) compact;


		template <typename Prefered, typename Levied>
		using prefer_t = std::conditional_t<sizeof(Prefered) == sizeof(Levied), Prefered, Levied>;

		//This helps control what type the given stored integral will be. handling change if it's 1/2/4/8 etc etc.
		//Basically the concept is the choosen storage type will pick an unsigned integer (or signed if they are equal and the prefered is signed) and will use this.
		// But, in the event it's not one of these straight more clear cut types, it will instead use an array for it's allocation.
		template<std::integral T , size_t Size, typename Outlier = std::array<uint8_t, Size>>
		using compact_rep = std::conditional_t<
			Size == 1, prefer_t<T, uint8_t>, std::conditional_t<
			Size == 2, prefer_t<T, uint16_t>, std::conditional_t<
			Size == 4, prefer_t<T, uint32_t>, std::conditional_t<
			Size == 8, prefer_t<T, uint64_t>, Outlier>
			>>>;
		
		template<size_t Size>
		using align_size = compact_rep<uint64_t, Size, bool>;


		//template<size_t Size>
		//using align_size = std::conditional_t<
		//	Size == 2, uint16_t, std::conditional_t<
		//	Size == 4, uint32_t, std::conditional_t<
		//	Size == 8, uint64_t, bool>>>;

		//template<typename T>
		//struct compact<T, sizeof(T)>;
	}

	
	namespace Compact
	{

		template<typename T, size_t Size>requires(sizeof(size_t) >= Size)
			struct alignas(detail::align_size<Size>) compact
		{
			std::array<uint8_t, Size> data;
		};

		compact<uint64_t, 5> test_compact;

		using UsingType = size_t;

		//To get the bits that shouldn't be accessed, get the maximum value of T, 
		// and bit shift it by the amount of bytes allowed to be had * 8 and that should do it. If an integer, simply the maximum value.

		//This is how you get the bits that shouldn't be active. If any process produces these, then that's an error.
		constexpr auto other = (2);
		constexpr UsingType cant_bits = max_value<UsingType> << (other * 8);

	}

	namespace ObjectWorksheet
	{
		namespace NewString
		{

			void Accept(const char8_t*)
			{

			}


			constexpr wchar_t t = '™';

			void test()
			{
				const wchar_t* test_v = L"thge";


				//std::strdup(test_v);
				//wcsdup
				//Need more than this.
				/*
				char *_strdup(
				   const char *strSource
				);
				wchar_t *_wcsdup(
				   const wchar_t *strSource
				);
				unsigned char *_mbsdup(
				   const unsigned char *strSource
				);
				//*/

				//Accept(test_v);
				std::basic_string<wchar_t> wide_str;
				std::basic_string<char8_t> smol_str;


				constexpr auto testrr = sizeof(decltype(wide_str)::value_type) > sizeof(decltype(smol_str)::value_type);

				using BiggerType = std::conditional_t<testrr, decltype(wide_str), decltype(smol_str) >;
				//wide_str;
			   //BiggerType comp_str = BiggerType{ smol_str };



			   //bool testComp = wide_str == smol_str;

			}
			//Not needed anymore.
			enum struct CharType : uint8_t
			{
				Byte,
				Word,
				DWord,
			};


			static_assert(std::is_unsigned_v<wchar_t>, "test failure");
			static_assert(alignof(wchar_t) == alignof(char16_t), "test failure");

			template <typename C>
			struct ptr//this is place holder.
			{
				const C* loc = nullptr;
			};

			namespace rgl
			{
				template< class T >
				concept character =
					std::is_same_v<char, T> ||
					std::is_same_v<char8_t, T> ||
					std::is_same_v<char16_t, T> ||
					std::is_same_v<char32_t, T> ||
					std::is_same_v<wchar_t, T> ||
					std::is_same_v<signed char, T> ||
					std::is_same_v<unsigned char, T>;


				//Here's the concept, I think I'm going to turn char, unsigned char, signed char, and wchar_t
				// into whatever char they're matched as. I'll make a custom type trait for this sort of thing.
				// it should match between files, but it means the basic string type will depend on your compiler.
				// But I mean, it always has

				//Correction, signed char remains.
			}


			template <rgl::character C>
			struct true_char
			{
				using type = C;
			};

			template <>
			struct true_char<unsigned char>
			{
				using type = char8_t;
			};

			template <>
			struct true_char<wchar_t>
			{
				using type = std::conditional_t<alignof(wchar_t) == alignof(char16_t), char16_t, char32_t>;
			};

			template <>
			struct true_char<char>
			{
				using type = std::conditional_t<std::is_unsigned_v<char>, char8_t, signed char>;
			};





			static void Imagine()
			{
				//std::use_facet<std::ctype<char
			}


			//do is spec of varient.
			template <specialization_of<std::variant> T>
			static constexpr auto free_space = 8 - sizeof(std::_Variant_index_t<std::variant_size_v<T>>);

			//These are the 4 types I'm keeping track of. Anything else will merely get turned into this.
			//signed char
			//char8_t
			//char16_t
			//char32_t




			using CharTypes = std::variant
				<
				ptr<signed char>,
				ptr<char8_t>,
				ptr<char16_t>,
				ptr<char32_t>
				>;

			//static_assert(std::is_same_v<char, signed char>);
			struct StringData
			{
				uint32_t size = 0;
			};
			REQ_OR_LESS_SIZE(StringData, free_space<CharTypes>);


			struct StringHelper
			{
				static constexpr auto offset = sizeof(CharTypes) - sizeof(StringData);

			protected:
				StringData& GetData()
				{
					auto a_this = (uintptr_t)this;

					return *reinterpret_cast<StringData*>(a_this + offset);
				}

				const StringData& GetData() const
				{
					auto a_this = (uintptr_t)this;

					return *reinterpret_cast<StringData*>(a_this + offset);
				}

				uint32_t& size()
				{
					return GetData().size;
				}

			public:



				uint32_t size() const
				{
					return GetData().size;
				}


				StringHelper()
				{
					size() = 0;
				}

				StringHelper(const RunDataHelper& other)
				{
					size() = 0;

				}


				StringHelper(RunDataHelper&& other)
				{
					size() = 0;
				}


				StringHelper& operator=(const RunDataHelper& other)
				{
					size() = 0;
					return *this;
				}



				StringHelper& operator=(RunDataHelper&& other)
				{
					size() = 0;
					return *this;
				}
			};



			template <rgl::character _Elem>
			struct CharHandle
			{
				using value_type = _Elem;

				//value_type

			};



			struct String
			{
				//This sort of thing will help converting one set to another. For now, I don't really need this sort of thing, and miss matches will be
				// rejected outright.
				//#include <codecvt>
				//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				//std::string narrow = converter.to_bytes(wide_utf16_source_string);
				//std::wstring wide = converter.from_bytes(narrow_utf8_source_string);

				//Unlike with arrays and such, strings will be treated as completely seperate types. This also helps not encounter any issues with generic implementations later.



				//I could get away with using a variant for this.
				void* ptr = nullptr;
				uint32_t size = 0;
				CharType type = CharType::Byte;


				bool operator==(const String& other)
				{
					if (type != other.type)
						return false;

					if (size != other.size)
						return false;


				}

				bool operator==(String&& other)
				{
					return operator==(other);
				}


				template <rgl::character Char>
				String(const Char* str)
				{

				}

				template <rgl::character Char>
				String(std::basic_string<Char> str)
				{

				}

			};
			REQUIRED_SIZE(String, 0x10);
		}

		class ClassStruct
		{
		
			AbstractTypePolicy* type = nullptr;

		private:
			union
			{
				uintptr_t			_raw = 0;
				Variable*			memberList;
				RuntimeVariable*	runtimeList;
			};
			
			size_t size = 0;

			//I'm thinking this is how I'm going to handle this. A union that helps contro it being a variable pointer and a runtime pointer. I can then 
			// switch what type it's percieved as.

			//This might make it a pain however.



			
		};
		

	}
	
	template<typename T, typename = void>
	struct any_value;

	template <typename T>
	struct any_value<T>
	{
		T& _value;

		any_value(const T& t) : _value{ t } {}
		any_value(T&& t) : _value{ t } {}
	};

	template <typename T>
	struct any_value<const T>
	{
		const T& _value;

		any_value(const T& t) : _value{ t } {}
		any_value(T&& t) : _value{ t } {}

	};

	void function(any_value<std::string> t)
	{

	}

	void other_func()
	{
		//function("nothing");
	}




	namespace SingletonWorksheet
	{
		template <typename T>
		struct Singleton
		{


			T* operator*()
			{
				//If the return of GetSingleton is a pointer give pointer.
				// if the result is a reference, turn reference into a pointer.

				decltype(auto) singleton = T::GetSingleton();

				using _Res = decltype(singleton);

				if constexpr (std::is_reference_v<_Res>)
				{
					return &singleton;
				}
				else// if constexpr (std::is_pointer_v<_Res>)//I'd like it if it doesn't return either you can't use this. Concept for that probs.
				{
					return singleton;
				}

			}

			const T& operator*() const
			{

			}
		};

		template <typename T>
		Singleton<T> singleton = Singleton<T>{};
	}

	namespace
	{
		struct TestInterface : public Interface, InterfaceSingleton<TestInterface>
		{
			uintptr_t Version() const override
			{
				return 0;
			}

			virtual void Printshit()
			{
				logger::critical("I'm printing stuff");
			}
		};

		INITIALIZE()
		{
			TestInterface* singleton = nullptr;
			if (auto result = InterfaceManager::RequestInterface(singleton, 0); result != Update::Match) {
				logger::info("failure {}", (int)result); return;
			}

			singleton->Printshit();


			TestInterface::instance->Printshit();



			ObjectPolicyManager* singleton2 = nullptr;
			if (auto result = InterfaceManager::RequestInterface(singleton2, 0); result != Update::Match) {
				logger::info("failure 2 {}", (int)result); return;
			}
			logger::info("obj_policy ver {}", singleton2->Version());

		};
	}
	
	
	struct Signature : public OverloadKey
	{
		//The match for this should be aimed to be as very little in terms of ambiguity as possible.
		//Thus, the stated project, script, and of course, the path.
		//The path for generics will probably simply be controlled by the amount of generic parameters it has, rather than the entires.
		// So no generic, no generic notation. Is generic, has generic notation. If it's the only one you can



		//Make these 2 a single function.
		Overload MatchFailure(OverloadFlag& flag)
		{
			logger::critical("Force failure");

			
			flag |= OverloadFlag::Failure;
			
			return {};
		}

		Overload MatchAmbiguous(OverloadFlag& flag)
		{
			logger::critical("Force ambiguous");

			
			flag |= OverloadFlag::Ambiguous;

			return {};
		}


		//Please note, this kind of match is what a clause should be doing.

		//This boolean needs to say if this failed to match, failed to be better, or resulted in ambiguity.
		Overload Match(OverloadClause* clause, ITypePolicy*, Overload*, OverloadFlag& a_flag) override
		{
			a_flag |= OverloadFlag::AllAccess;


			if (clause->PreEvaluate(result, parameters.size(), 0, a_flag) == false) {
				return MatchFailure(a_flag);
			}


			//I want to phase out of function. Maybe combine it with prev in some way.

			//Make a copy as to not completely mutate this.
			OverloadFlag flag = a_flag;


			Overload overload;


			OverloadEntry tar = clause->EvaluateEntry2(target, nullptr, -1, -1, flag);


			if (flag & OverloadFlag::Failure || tar.convertType != ConvertResult::Exact)
				return MatchFailure(a_flag);

			overload.target = tar.type;
			overload.clause = clause;

			constexpr auto offset_placeholder = 0;

			int winner = 0;

			for (int i = 0; i < parameters.size(); i++)
			{
				QualifiedType input = parameters[i];

				OverloadEntry entry = clause->EvaluateEntry2(input, nullptr, offset_placeholder, i, flag);

				if (flag & OverloadFlag::Failure || tar.convertType != ConvertResult::Exact)
					return MatchFailure(a_flag);



				//entry.funcs = conversion;
				//entry.type = input;
				//entry.index = index;


				overload.given.push_back(entry);
			}

			return overload;

		}



		QualifiedType result;
		QualifiedType target;
		
		std::vector<QualifiedType>				parameters;
		
	};
	
	//TODO: Move to RGL
	template <class T> struct extract_class { using type = T; };
	template <class T> struct extract_class<T&> { using type = T; };
	template <class T> struct extract_class<T const&> { using type = T; };
	template <class T> struct extract_class<T&&> { using type = T; };
	template <class T> struct extract_class<T*> { using type = T; };
	template <class T> struct extract_class<T const*> { using type = T; };

	static_assert(std::is_same_v<int const, const int>);

	
	enum struct SignatureEnum
	{
		Result,
		Target, 
		Argument,
	};

	using INT = std::remove_const_t<const int&>;
	template <SignatureEnum T, typename E, typename... Next>
	bool ProcessEntry(Signature& sign)
	{
		//In order to handle specific reference types, or specific qualifiers, there will be a wrapper type to handle it.
		// I'll just unpack it when the time comes. This would mostly be used for the target of a function.
		// under these situations, it will use the suggested  qualifiers rather than the the true ones.

		//If an the last entry is the callInfo, ignore it and move on. If it's callinfo but not last, do not compile.


		if constexpr (std::is_same_v<E, Void> && T == SignatureEnum::Argument) {
			//Do not compile actually.
			return false;
		}


		//Need to save this for later.
		//if constexpr (Tar && !std::is_reference_v<E>) {
		//	return false;
		//}

		constexpr auto next_size = sizeof...(Next);

		using _Pointless = std::remove_pointer_t<E>;
		using _Refless = std::remove_const_t<std::remove_reference_t<E>>;
		using _Naked = std::remove_pointer_t<_Pointless>;//Might be better to get the underlying value of the thing.

		QualifiedType& entry = T == SignatureEnum::Result ?
			sign.result : T == SignatureEnum::Target ?
			sign.target : sign.parameters.emplace_back();
			
		logger::info("increase? {}", sign.parameters.size());
		entry.policy = FetchVariableType<_Refless>();

		if constexpr (std::is_const_v<_Naked>){
			entry.flags |= Qualifier::Const;
		}

		//For right now it really doesn't matter.
		if constexpr (std::is_reference_v<E>) {
			entry.flags |= Qualifier::RefL;
		}



		if constexpr (next_size)
		{
			constexpr SignatureEnum Enum = T == SignatureEnum::Result ?
				SignatureEnum::Target : T == SignatureEnum::Target ?
				SignatureEnum::Argument : T;


			return ProcessEntry<Enum, Next...>(sign);
		}
		else
		{
			return true;
		}
	}
	
#define TESTNAME CONCAT(Test,__COUNTER__)

	void SCRAPNAME(void(*infoke)())
	{
		
	}
	void Scrapname()
	{
		
	}
	struct Callish
	{
		void operator () ()
		{

		}
	};


	struct Scrap
	{
		//inline static Callish name;
		static void name(int)
		{

		}
		static void name()
		{

		}
	};

	constexpr bool is_thing = std::is_polymorphic_v < std::function<void()>>;
	
	/*
	struct TargetEnvironment_nixed
	{
		enum Flag : uint8_t
		{
			None = 0 << 0,
			Explicit = 1 << 0,
		};


		//Size isn't used to its fullest

		//So issue, this actually used to use a solution, and rightfully so. Target object
		// as itself 





		Environment* target = nullptr;
		TargetObject* prev = nullptr;
		Flag			flag = Flag::Explicit;

		//Purges the use of assigning TargetObjects. This is used when returning or when assigning for implicit
		// constuction knowledge.
		TargetObject* GetCallTarget()
		{
			if (this) {
				return flag & Flag::Assign ? prev->GetCallTarget() : this;
			}

			return nullptr;
		}

		//Reverse of call target, this gets the target of an assignment or return. Useful for ternary statements
		TargetObject* GetAssignTarget()
		{
			if (this) {
				return flag & Flag::Assign ? this : prev->GetAssignTarget();
			}

			return nullptr;
		}

		TargetObject(Solution& t, TargetObject* p, Flag f) : target{ &t }, prev{ p }, flag{ f }
		{

		}

		TargetObject(Solution& t, TargetObject* p = nullptr, bool b = false) : target{ &t }, prev{ p }, flag{ b ? Explicit : None }
		{

		}
	};
	//*/






	struct TestA {
		struct TestB {
			struct TheTest
			{
				static int last;
			};
			TheTest GetTest()
			{
				return {};
			}
		};
	};


	struct _SIZE
	{
		int a, b, c;
	};
	void TESTNAME()
	{
		//Scrap::name::(int);
		int n{ 1 };

		_SIZE size(1, 2, 3);

		//TestA::TestB::GetTest()::last;
		Scrapname();
		Record empty;
		GetPolicyFromSpecifiers(empty, nullptr);
	}



	
	struct OtherTest
	{

	};
	struct Test
	{
		operator int() { return 0; }
		template<std::floating_point T>
		explicit operator T() { return 0; }
		
		explicit operator T() { return {}; }
	};

	


	void TEST()
	{
#define unique_name CONCAT(test, __COUNTER__)
		constexpr bool test4 = specialization_of<VariableValue, std::variant>;
		constexpr bool test = is_any_convertible_v<float, int, int, int, int>;
		constexpr bool test2 = is_variant_convertible_v<VariableValue, float>;
		constexpr bool test3 = variant_convertible_to<VariableValue, double>;
		//This breaks despite the fact it should be hable to handle this. Please consult.
		//constexpr bool test5 = variant_convertible_to<double, VariableValue>;

		constexpr bool test226 = stl::convertible_from<double, Variable>;
		constexpr bool test6 = stl ::convertible_to<Variable, double>;

	

		constexpr bool test7 = std::convertible_to<Test, int>;
		constexpr bool test8 = std::convertible_to<Test, float>;
		constexpr bool test9 = std::convertible_to<Test, T>;
		constexpr bool test10 = std::is_convertible_v<Number, double>;
		
		constexpr bool _1 = __is_convertible_to(Number, double);
		constexpr bool _2 = stl::castable_to<Number, double>;
		constexpr auto test11 = convertible_variant_index<VariableValue, double>::value;
		
		
		//Test2<double>();
		//Test3<double>();
		Variable a;
		a.CanCastTo<double>();
		//GetStorageType<String>();
		Unvariable<double>{}(&a);
		static_cast<double>(a);


	}


	struct PathParser : public LEX::Impl::ParseModule, public LEX::Impl::IdenDeclBoilerPlate
	{
		//The idea of this is it's a simple parser that acts like the identifier parser, but will handle this in a way that can handle
		bool CanHandle(LEX::Impl::Parser* parser, Record* target, LEX::Impl::ParseFlag flag) const override
		{
			return true;
		}

		bool IsAtomic() const override
		{
			return true;
		}


		Record _HandleThis(LEX::Impl::Parser* parser)
		{
			RecordData next = parser->next();
			next.GetTag() = parse_strings::this_word;
			return LEX::Impl::Parser::CreateExpression(next, SyntaxType::Field);
		}


		Record HandleToken(LEX::Impl::Parser* parser, Record*) override
		{

			return _HandlePath(parser, SyntaxType::ProjectName);

		}
		// Need some boilerplate resolver.
		std::string_view GetModuleName() override
		{
			return typeid(PathParser).name();
		}

		bool ContextAllowed(LEX::Impl::ProcessContext*, LEX::Impl::ProcessChain*) override
		{
			//This prevents anything from following it up for the most part. This shit is a one man show!
			// If this causes any issues with parsers I may use later, feel free to make a variable to help handle when this is allowed be handled.
			return false;
		}
	};



	struct Nameless//This goes in the project manager. Make that shit an interface and give it a source
	{
		enum Enum
		{
			NoneElement,
			TypeElement,
			FuncElement,
			GlobElement,
		};




		inline static std::unordered_map<size_t, std::pair<Element*, Enum>> _lookupMap;



		static Element* LookupElement(size_t hash, Enum type)
		{
			auto it = _lookupMap.find(hash);

			if (_lookupMap.end() == it || it->second.second == type)
				return nullptr;

			return it->second.first;
		}

		static void SaveElement(Element* elem, size_t hash, Enum type)
		{

			_lookupMap[hash] = std::make_pair(elem, type);

		}

		static size_t HashPath(std::string& path)
		{
			//TODO: -Check: I think unordered map already hashes stuff so maybe not needed. Still will handle later
			std::hash<std::remove_reference_t<decltype(path)>> hasher{};

			return hasher(path);

		}

		static Element* GetElementFromPath(std::string path, Enum elem)
		{
			//right now, linking isn't really set up so you know.
			if (1 || Component::HasLinked(LinkFlag::Declaration) == false){
				//Tell that this is too early to be valid.
				return nullptr;
			}


			auto hash = HashPath(path);

			auto element = LookupElement(hash, elem);


			if (!element)
			{
				Record record = LEX::Impl::Parser__::CreateSyntax<PathParser>(path);

				//<!> After parsing is done, the top most type needs to be set to the standards of what it's expecting.

				//From here, use the path functions that are in environment.
				// The project should be found from the first part, and from there we should just keep the environ search should finish it out.

				switch (elem)
				{
					//Do the search for each type here.
				case Enum::FuncElement:
				case Enum::TypeElement:
				case Enum::GlobElement:
				default:
					element = nullptr; break;
				}


				if (element) {
					SaveElement(element, hash, elem);
				}

			}

			return element;
		}

		static FunctionBase* GetFunctionFromPath(std::string path)
		{
			if (auto elem = GetElementFromPath(path, FuncElement); elem)
				return static_cast<FunctionBase*>(elem);

			return nullptr;
		}

		static FunctionBase* GetFunctioFromPath(std::string path)
		{
			if (auto elem = GetElementFromPath(path, FuncElement); elem)
				return static_cast<FunctionBase*>(elem);

			return nullptr;
		}





		static PolicyBase* GetTypeFromPath(std::string path)
		{
			if (auto elem = GetElementFromPath(path, TypeElement); elem)
				return static_cast<PolicyBase*>(elem);

			return nullptr;
		}

		static Global* GetGlobalFromPath(std::string path)
		{
			if (auto elem = GetElementFromPath(path, GlobElement); elem)
				return static_cast<Global*>(elem);

			return nullptr;
		}

	};


	
	template <typename R, typename... Args>
	bool RegisterFunction(R (*func)(Args...), FunctionBase* base)
	{
		//Currently, the only way to properly handle the system is by making a unique lambda each time a new template is created. But needless to say,
		// cant do that.

		//I think something I can do is that optionally a 64 bit context code can be given when making a procedure. This allows me to just simply pull what's needed.
		// So I think that might be a proceedure thing idk. Something to make context of. But something fast. Definitely not something like what native function
		// would have been.

		//The context thing is a good idea, it can possibly sort out which overload is being set up, if someone is using it pure (which one shouldn't.
		// Long story short it's the address to use.

		Signature sign{};

		//bool processed = ProcessEntry<true, R, Args...>(sign);
		bool processed = ProcessEntry<SignatureEnum::Result, R, Args...>(sign);

		if (!processed)
			throw nullptr;

		OverloadFlag flag = OverloadFlag::None;

		auto overload = sign.Match(base, nullptr, nullptr, flag);

		if (flag & OverloadFlag::Failure)
			logger::info("FAILED TO MATCH");
		else
			logger::info("SUCCESS TO MATCH");

		//static auto dispatch = new BasicDispatcher(func, base);
		auto result = BasicDispatcher<R, Args...>::Create(func, base);

		return result;
	}

	//thing to get function here.

	template <typename R, typename... Args>
	bool RegisterFunction(R (*func)(Args...), std::string path)
	{
		FunctionBase* base = Nameless::GetFunctionFromPath(path);

		return RegisterFunction(func, base);
	}

	bool RegisterFunction(Procedure procedure, std::string path)
	{
		FunctionBase* func = Nameless::GetFunctionFromPath(path);

		return ProcedureHandler::instance->RegisterFunction(procedure, func);
	}


#include <io.h>
#include <fcntl.h>





	INITIALIZE()
	{
		std::string message = "Something {} {} {}";
		std::source_location loc = std::source_location::current();
		report::debug("Something new {} {} {}", 1, 2, 3);
		report temp_switch{ IssueType::Compile };
		report::info("Something else {} {} {}", 3, 2, 1);

		report::message::info(1032, 3, 2, 1);
		
		//constexpr auto v = "こんにちは世界";
		//_setmode(_fileno(stdout), _O_U16TEXT);
		//SetConsoleOutputCP(CP_UTF8);
		//std::wcout << "こんにちは世界";

		//report::message::debug(v);
		if constexpr (false)
			report::parse::critical("Shit ends here");
		//repTest1("", 12, 23);

		assert(1);

		
	}

	INITIALIZE()
	{
		report::info("Something last {} {} {}", 3, 2, 1);
	}
}


namespace fmt
{
	//Really want this to work

	template <std::convertible_to<std::string> T>
	struct formatter
	{

		template <class ParseContext>
		constexpr auto parse(ParseContext& a_ctx)
		{
			return a_ctx.begin();
		}

		template <class FormatContext>
		auto format(const T& v, FormatContext& a_ctx) const
		{
			return static_cast<std::string>(v);
			//auto* info = RE::ActorValueList::GetSingleton()->GetActorValue(a_actorValue);
			//return fmt::format_to(a_ctx.out(), "{}", info ? info->enumName : "None");
		}
	};

	void Test()
	{
		LEX::Number num{ 1.5f };

		logger::info("{} test", num);
	}
}
