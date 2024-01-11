#pragma once

#include "Target.h"
#include "ITypePolicy.h"
#include "OperandType.h"
#include "Operand.h"
namespace LEX
{
	struct Solution : public Operand
	{
		ITypePolicy* policy{};		//I think I use an ITypePolicy? That's sorta the rule here.
		//Operand result{};
		
		//Target value;//holds the accessing value, index, pointer, whatever.
		
		//OperandType type;//denotes what the return type should be.
		//Should come with additional flags and values.
		// Like, is it an lhs, or rhs that's expected to be pushed? Of which, it has to be one. Just one, not dynamic.
		// this can control what it believes can occur to it.

		//OR, is the expected value a constant? All very important questions.

		//Need some way to convey if this is a ref or not. Might be if it's using registers or not, 
		// but might have some flags.

		bool IsReadOnly() const
		{
			return false;
		}

		bool IsReference() const
		{
			return false;
		}

		Solution() = default;

		Solution(ITypePolicy* a_policy, OperandType a_type, Target a_target) :
			policy{ a_policy }, Operand{ a_target, a_type }
		{
			//RGL_LOG(trace, "sol ctor ops {}", a_type);
		}
	};
}