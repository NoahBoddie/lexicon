#pragma once

#include "Lexicon/Engine/Register.h"
#include "Lexicon/Variable.h"
#include "Lexicon/Engine/ITypeInfoImpl.h"
#include "Lexicon/Engine/IFunctionImpl.h"
#include "Lexicon/MemberPointer.h"
#include "OperandType.h"
#include "Literal.h"

namespace LEX
{

	using Index = uint64_t;
	using Differ = int64_t;

	struct IGlobal;

	struct Target
	{
		//Needs to be able to handle literals, which are not variable pointers.
		constexpr Target() {}
		constexpr Target(int arg) : raw{ arg } {}//doesn't matter which it is.
		constexpr Target(Differ arg) : differ{ arg } {}
		constexpr Target(Index arg) : index{ arg } {}
		constexpr Target(Register arg) : reg{ arg } {}
		constexpr Target(Literal arg) : lit{ arg } {}
		constexpr Target(IGlobal* arg) : glob{ arg } {}
		constexpr Target(ITypeInfo* arg) : type{ arg } {}
		constexpr Target(IFunction* arg) : func{ arg } {}
		constexpr Target(ICallableUnit* arg) : call{ arg } {}
		constexpr Target(MemberPointer arg) : member{ arg } {}
	
		template <OperandType Type, typename T>
		bool Get(T& out)
		{
			if constexpr (Type == OperandType::None) {
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
		bool Get<OperandType::Register, Register>(Register& out)
		{
			out = reg;
			return true;
		}


		template <>
		bool Get<OperandType::Literal, Literal>(Literal& out)
		{
			out = reg;
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
			Differ differ;
			Index index;
			Register reg;
			Literal lit;
			IGlobal* glob;
			ITypeInfo* type;
			IFunction* func;
			ICallableUnit* call;
			MemberPointer member;

		};

	};

}
