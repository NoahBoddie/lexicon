#pragma once


#include "Lexicon/Qualifier.h"
#include "Lexicon/AnnotatedType.h"
//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"


//*src
#include BASIC_NAME(Type)

namespace LEX
{
	struct IType;
	struct OverloadCode;



	struct QualifiedType : public AnnotatedType
	{
		using AnnotatedType::AnnotatedType;
		QualifiedType(const AnnotatedType& other) : AnnotatedType {other}{}

		using AnnotatedType::operator=;


		ConvertResult IsQualified(const QualifiedType& other, ConversionFlag flags, Conversion** out) const;



		//TODO: IsCovertToQualfied needs to hold an IType to see if it has permission to this conversion. How I'd do that, is kinda hard.
		ConvertResult IsConvertToQualified(const QualifiedType& other, BasicType* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const;
	};
}