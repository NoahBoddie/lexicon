#pragma once

#include "Register.h"
#include "Variable.h"
#include "ITypePolicy.h"
#include "IFunction.h"
#include "MemberPointer.h"
#include "OperandType.h"
#include "Literal.h"

namespace LEX
{

	using Index = uint64_t;

	struct Target
	{
		//Needs to be able to handle literals, which are not variable pointers.
		constexpr Target() {}
		constexpr Target(int64_t arg) : raw{ arg } {}
		constexpr Target(Index arg) : index{ arg } {}
		constexpr Target(Register arg) : reg{ arg } {}
		constexpr Target(Literal arg) : lit{ arg } {}
		constexpr Target(Variable* arg) : var{ arg } {}
		constexpr Target(ITypePolicy* arg) : type{ arg } {}
		constexpr Target(IFunction* arg) : func{ arg } {}
		constexpr Target(MemberPointer arg) : member{ arg } {}

		union
		{
			int64_t raw = full_value<int64_t>;
			Index index;//Make index a type itself.
			Register reg;
			Literal lit;
			Variable* var;
			ITypePolicy* type;
			IFunction* func;
			MemberPointer member;

		};
		
		template <OperandType Type, typename T>
		bool Get(T& out)
		{
			if constexpr (Type == OperandType::None) {
				return false;
			}
			else {
				out = reinterpret_cast<T&>(raw);
				return true;

			}

		}

		template <>
		bool Get<OperandType::Function, IFunction*>(IFunction*& out)
		{
			out = func;
			return true;
		}

		template <>
		bool Get<OperandType::Index, Index>(Index& out)
		{
			out = index;
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
		bool Get<OperandType::Variable, Variable*>(Variable*& out)
		{
			out = var;
			return true;
		}

		template <>
		bool Get<OperandType::Type, ITypePolicy*>(ITypePolicy*& out)
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



	};

}
