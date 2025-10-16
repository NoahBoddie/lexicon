#pragma once
#include "Lexicon/Engine/Target.h"
#include "Lexicon/Engine/Register.h"
#include "Lexicon/Engine/OperandType.h"


#include "Lexicon/RuntimeVariable.h"

namespace LEX
{
	class Runtime;

	enum struct DynamicIndex
	{
		None,
		Argument,
		Variable,
		Parameter,
	};

	struct Operand : public Target
	{


		//private;
		Target& data()
		{
			return *this;
		}

		OperandType opType{ OperandType::Total };

		constexpr OperandType type() const noexcept
		{
			return opType & ~OperandType::DynamicIndex;
		}

		bool IsVariable() const
		{
			return type() == OperandType::Variable;//Account for the index type?
		}

		bool IsParameter() const
		{
			return type() == OperandType::Parameter;//Account for the index type?
		}


		template <typename T, OperandType Type>
		T _InternalGet()
		{
			//This entire thing needs
			if constexpr ((uint8_t)Type < (uint8_t)OperandType::Invalid)
			{
				if (Type == type())
				{
					T result{};
					//Error message should probably be different here.
					if (data().Get<Type>(result) == false) {
						report::runtime::critical("Failure to get Target, OperandType {}", magic_enum::enum_name(Type));
					}

					return result;

				}
				else
				{
					return _InternalGet<T, OperandType((uint8_t)Type + 1)>();
				}

			}

			//error here.
			report::compile::critical("Unexpected type in Operand, cannot get value.");

			return {};
		}

		template <typename T, OperandType Type>
		bool _InternalGet(T& result)
		{
			//This entire thing needs
			if constexpr ((uint8_t)Type < (uint8_t)OperandType::Invalid)
			{
				if (Type == type())
				{
					//Error message should probably be different here.
					return data().Get<Type>(result);

				}
				else
				{
					return _InternalGet<T, OperandType((uint8_t)Type + 1)>();
				}

			}

			//error here.
			report::compile::critical("Unexpected type in Operand, cannot get value.");

			return false;
		}

		//Somehow, Operand::Get doesn't seem to crash when getting the wrong underlying type.
		template <typename T>
		T Get()
		{
			return _InternalGet<T, OperandType::None>();
		}

		template <typename T>
		bool Get(T& value)
		{
			return _InternalGet<T, OperandType::None>(value);
		}


		template <OperandType Type, typename T>
		bool Equals(T value)
		{
			if (Type != type())
				return false;

			T result{};

			if (data().Get<Type>(result) == false)
				return false;

			return result == value;
		}

		Index GetIndex(Runtime* runtime);

		Differ GetDiffer(Runtime* runtime);

		Number GetConstant();
		
		//I'd like to rework some of the names, GetValueNumber gives a number, get value the runtime var. same idea for getNumber, where it's GetSubjectNumber


		
		RuntimeVariable GetValue(Runtime* runtime);
		RuntimeVariable& AsValue(Runtime* runtime);

		Number GetValueNumber(Runtime* runtime)
		{
			return GetValue(runtime)->AsNumber();
		}
		Number GetSubjectNumber(Runtime* runtime)
		{
			return GetSubject(runtime)->AsNumber();
		}


		RuntimeVariable& AsSubject(Runtime* runtime);
		RuntimeVariable GetSubject(Runtime* runtime);

		constexpr OperandType GetDynamicIndex() const noexcept
		{
			return opType & OperandType::DynamicIndex;
		}

		RuntimeVariable& CheckDynamicSubject(Runtime* runtime, RuntimeVariable& subject);
		Variable CopyVariable(Runtime* runtime);


		RuntimeVariable& AsVariable(Runtime* runtime);

		//Returns a RuntimeVariable that's either a viable reference to a Variable, or a pure variable if able
		RuntimeVariable GetVariable(Runtime* runtime);


		RuntimeVariable ObtainVariable(Runtime* runtime);
		RuntimeVariable& ObtainAsVariable(Runtime* runtime);

		//This should be used later to retrieve functions that need to be called. Also to be used to get specific functions I'd presume
		//Function* GetFunction(Runtime* runtime);

		TypeInfo* GetTypeInfo(Runtime* runtime);

		constexpr bool IsTemporary() const noexcept
		{
			switch (type())
			{
			case OperandType::Register:
			case OperandType::Literal:
				//case OperandType::Argument://Argument is pretty temporary
				return GetDynamicIndex();
			default:
				return false;
			}
		}

		constexpr bool IsEmpty() const noexcept { return type() == OperandType::None; }


		constexpr static Operand None()
		{
			return Operand{ 0, OperandType::None };
		}

		constexpr Operand AsDynamic(DynamicIndex type) const noexcept
		{
			Operand copy = *this;

			//If not a constexpr should be able to report an incompatible type if used with something like a literal

			switch (type)
			{
			case DynamicIndex::Variable:
				copy.opType |= OperandType::VariableIndex;
				break;
			case DynamicIndex::Argument:
				copy.opType |= OperandType::ArgumentIndex;
				break;
			case DynamicIndex::Parameter:
				copy.opType |= OperandType::ParameterIndex;
				break;
			}

			return copy;
		}

		constexpr Operand() = default;
		constexpr Operand(Target d, OperandType t, DynamicIndex ind = DynamicIndex::None) : Target{ d }, opType{ t }
		{
			switch (ind)
			{
			case DynamicIndex::Variable:
				opType |= OperandType::VariableIndex;
				break;
			case DynamicIndex::Argument:
				opType |= OperandType::ArgumentIndex;
				break;
			case DynamicIndex::Parameter:
				opType |= OperandType::ParameterIndex;
				break;
			}
		}
	};

}
