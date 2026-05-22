#pragma once

#include "Lexicon/Engine/Register.h"
#include "Lexicon/Variable.h"
#include "Lexicon/Engine/ITypeInfoImpl.h"
#include "Lexicon/Engine/IFunctionImpl.h"
#include "Lexicon/Engine/Conversion.h"
#include "Lexicon/MemberPointer.h"
#include "OperandType.h"
#include "Literal.h"
namespace LEX
{

	using Index = uint64_t;
	using Differ = int64_t;
	
	struct TypeInfo;

	struct IndexSplit
	{
		uint32_t first;
		uint32_t second;
	};
	
	struct DifferSplit
	{
		int32_t first;
		int32_t second;
	};


	struct RoutineBase;

	struct IGlobal;

	struct Target
	{
		//Needs to be able to handle literals, which are not variable pointers.
		constexpr Target() {}
		constexpr Target(int arg) : raw{ arg } {}//doesn't matter which it is.
		template<typename T>
		constexpr Target(T arg) requires (std::is_enum<T>::value) : raw{(uint64_t)arg} {}//doesn't matter which it is.
		constexpr Target(uint32_t arg) : raw{ arg } {}//doesn't matter which it is.
		constexpr Target(IndexSplit arg) : inSplit{ arg } {}
		constexpr Target(DifferSplit arg) : difSplit{ arg } {}
		constexpr Target(Differ arg) : differ{ arg } {}
		constexpr Target(Index arg) : index{ arg } {}
		constexpr Target(Register arg) : reg{ arg } {}
		constexpr Target(const Literal* arg) : lit{ arg } {}
		constexpr Target(IGlobal* arg) : glob{ arg } {}
		constexpr Target(ITypeInfo* arg) : type{ arg } {}
		constexpr Target(IFunction* arg) : func{ arg } {}
		constexpr Target(Converter_ arg) : convert{ arg } {}
		constexpr Target(ICallableUnit* arg) : call{ arg } {}
		constexpr Target(RoutineBase* arg) : routine{ arg } {}
		constexpr Target(MemberPointer arg) : member{ arg } {}
	
		template <OperandType Type, typename T>
		bool Get(T& out)
		{
			if constexpr (Type == OperandType::None || Type == OperandType::Enum && std::is_enum_v<T>) {
				out = reinterpret_cast<T&>(raw);
				return true;
			}
			else {
				return false;
			}

		}

		template <>
		bool Get<OperandType::Function, IFunction*>(IFunction*& out)
		{
			out = func;
			return true;
		}


		template <>
		bool Get<OperandType::Routine, RoutineBase*>(RoutineBase*& out)
		{
			out = routine;
			return true;
		}


		template <>
		bool Get<OperandType::Converter, Converter_>(Converter_& out)
		{
			out = convert;
			return true;
		}

		template <>
		bool Get<OperandType::Callable, ICallableUnit*>(ICallableUnit*& out)
		{
			out = call;
			return true;
		}

		template <>
		bool Get<OperandType::Index, Index>(Index& out)
		{
			out = index;
			return true;
		}



		template <>
		bool Get<OperandType::Variable, Index>(Index& out)
		{
			out = index;
			return true;
		}

		template <>
		bool Get<OperandType::Value, Index>(Index& out)
		{
			out = index;
			return true;
		}

		template <>
		bool Get<OperandType::Parameter, Index>(Index& out)
		{
			out = index;
			return true;
		}


		template <>
		bool Get<OperandType::Argument, Index>(Index& out)
		{
			out = index;
			return true;
		}


		template <>
		bool Get<OperandType::Differ, Differ>(Differ& out)
		{
			out = differ;
			return true;
		}

		template <>
		bool Get<OperandType::IndexSplit, IndexSplit>(IndexSplit& out)
		{
			out = inSplit;
			return true;
		}


		template <>
		bool Get<OperandType::DifferSplit, DifferSplit>(DifferSplit& out)
		{
			out = difSplit;
			return true;
		}


		template <>
		bool Get<OperandType::Register, Register>(Register& out)
		{
			out = reg;
			return true;
		}


		template <>
		bool Get<OperandType::Literal, const Literal*>(const Literal*& out)
		{
			out = lit;
			return true;
		}

		template <>
		bool Get<OperandType::Global, IGlobal*>(IGlobal*& out)
		{
			out = glob;
			return true;
		}

		template <>
		bool Get<OperandType::Type, ITypeInfo*>(ITypeInfo*& out)
		{
			out = type;
			return true;
		}

		template <>
		bool Get<OperandType::Member, MemberPointer>(MemberPointer& out)
		{
			out = member;
			return true;
		}


	public:
		union
		{
			int64_t raw = static_cast<int64_t>(-1);
			DifferSplit difSplit;
			IndexSplit inSplit;
			Differ differ;
			Index index;
			Register reg;
			const Literal* lit;
			IGlobal* glob;
			ITypeInfo* type;
			IFunction* func;
			Converter_ convert;
			ICallableUnit* call;
			RoutineBase* routine;
			MemberPointer member;

		};

	};

}
