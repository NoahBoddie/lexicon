#pragma once
#include "Target.h"
#include "Lexicon/Impl/Register.h"
#include "Lexicon/Impl/OperandType.h"


#include "RuntimeVariable.h"

namespace LEX
{
	class Runtime;

	struct Operand : public Target
	{
		//private;
		Target& data()
		{
			return *this;
		}

		OperandType type{ OperandType::Total };


		template <typename T, OperandType Type>
		T _InternalGet()
		{
			//This entire thing needs
			if constexpr ((uint8_t)Type < (uint8_t)OperandType::Invalid)
			{
				if (Type == type)
				{
					T result{};
					//Error message should probably be different here.
					if (data().Get<Type>(result) == false) {	
						report::runtime::critical("Failure to get Target, OperandType {}", (uint8_t)Type);
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

		//Somehow, Operand::Get doesn't seem to crash when getting the wrong underlying type.
		template <typename T>
		T Get()
		{
			return _InternalGet<T, OperandType::None>();
		}


		template <OperandType Type, typename T>
		bool Equals(T value)
		{
			if (Type != type)
				return false;

			T result{};
			
			if (data().Get<Type>(result) == false)
				return false;

			return result == value;
		}


		Variable CopyVariable(Runtime* runtime);


		RuntimeVariable& AsVariable(Runtime* runtime);


		RuntimeVariable GetVariable(Runtime* runtime);


		RuntimeVariable ObtainVariable(Runtime* runtime);
		RuntimeVariable& ObtainAsVariable(Runtime* runtime);


		constexpr Operand() = default;
		constexpr Operand(Target d, OperandType t) : Target{ d }, type{ t }
		{

		}
	};

}
