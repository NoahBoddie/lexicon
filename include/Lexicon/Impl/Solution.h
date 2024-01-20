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


		//Solution needs some qualifiers, but to that it needs 3 types
		// PassByValue-Copy it, and send it. Invalid to be used where PassByRefR/L are used.
		// PassByRefR-Send the reference of a temporary object. Invalid to use on PassByRefL.
		// PassByRefL-Send the reference of a persistent object. Valid for use anywhere.

		//-Examples of L refs are globals, and examples of R refs are variables or parameters.
		//-To specify the above better, returning will always only be able to pass an L ref, never a R ref
		// The reason why this would be a problem is if one where to send the reference of a parameter
		// that's already been deallocated.
		//That being said, arguments for functions are a good example of L refs, because as long as the called function
		// is concerned, those refs are always valid. Yet, it will not be able to trade them out in a return (which 
		// expects data that will exist indefinitely).

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