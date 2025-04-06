#pragma once

#include "Lexicon/Engine/Target.h"
#include "Lexicon/Engine/AbstractType.h"
#include "Lexicon/Engine/OperandType.h"
#include "Lexicon/Engine/Operand.h"


//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"
#include "Lexicon/Qualifier.h"


#include "Lexicon/Engine/QualifiedType.h"

namespace LEX
{
	struct Solution : public Operand, public QualifiedType
	{

		using QualifiedType::operator=;


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
			//TODO: Literals should be qualified as const. If they are not please address this.
			return type == OperandType::Literal || IsConst();
		}

		



		
		Solution() = default;


		constexpr Solution(const QualifiedType& a_type, OperandType a_opType, Target a_target) noexcept :  Operand{ a_target, a_opType }, QualifiedType { a_type }
		{
		}

		using Operand::operator=;
		/*
		constexpr Solution& operator= (const Operand& rhs) noexcept
		{
			Operand::operator=(rhs);
			return *this;
		}

		constexpr Solution& operator= (Operand&& rhs) noexcept
		{
			return Solution::operator=(rhs);
		}
		//*/
		
	};
}