#pragma once

#include "Target.h"
#include "ITypePolicy.h"
#include "OperandType.h"
#include "Operand.h"


//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"
#include "Qualifier.h"

#include "ParamInput.h"

#include "QualifiedType.h"

namespace LEX
{
	struct Solution : public Operand, public QualifiedType
	{


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
			return type == OperandType::Literal || !FilterEquals<Qualifier::Volatility_>(flags, Qualifier::Modable);
		}

		bool IsReference() const
		{
			return false;
		}

		Solution() = default;


		Solution(ITypePolicy* a_policy, OperandType a_type, Target a_target) :  Operand{ a_target, a_type }
		{
			policy = a_policy;
		}


		constexpr Solution& operator= (Operand& rhs) noexcept
		{
			Operand::operator=(rhs);
			return *this;
		}

		constexpr Solution& operator= (Operand&& rhs) noexcept
		{
			return Solution::operator=(rhs);
		}

		
	};
}