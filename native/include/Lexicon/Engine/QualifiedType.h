#pragma once


#include "Lexicon/Qualifier.h"
#include "Lexicon/AnnotatedType.h"
//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"


//*src
#include "Lexicon/TypeInfo.h"

namespace LEX
{
	struct ITypeInfo;
	struct OverloadCode;



	struct QualifiedType : public AnnotatedType
	{
		using AnnotatedType::AnnotatedType;
		QualifiedType(const AnnotatedType& other) : AnnotatedType {other}{}

		using AnnotatedType::operator=;



		//For an assign to take place, the left must be assignable

		//Can you assign a whole new value
		constexpr bool IsAssignable() const noexcept
		{
			if (IsReadonly() == true)
				return false;

			if (IsConst() && policy->IsValueType() == true)
				return false;

			return true;

		}
		




		ConvertResult IsQualified(const QualifiedType& to_left, ConversionFlag flags, Conversion** out) const;



		//TODO: IsCovertToQualfied needs to hold an ITypeInfo to see if it has permission to this conversion. How I'd do that, is kinda hard.
		ConvertResult IsConvertToQualified(const QualifiedType& to_left, ITypeInfo* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const;
	};
}