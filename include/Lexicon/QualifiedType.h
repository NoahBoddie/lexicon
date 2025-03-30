#pragma once


#include "Qualifier.h"
//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"


//*src
#include BASIC_NAME(Type)

namespace LEX
{
	struct IType;
	struct OverloadCode;










	//TODO: Merge Qualified Type with it's qualifiers
	struct QualifiedType : public Qualifier
	{
		//Within the compiling process, this is more often than not what should be used.
		//TODO: QualifiedType replaces param input.

		QualifiedType() = default;

		QualifiedType(std::nullptr_t) {}

		explicit QualifiedType(BasicType* p) : policy{ p } {}
		QualifiedType(BasicType* p, Qualifier q) : policy{ p }, Qualifier{ q } {}

		using Qualifier::operator=;//(const Qualifier& other) { __super::operator=(other); return *this; }


	//private://Temporary to sus out where policy is expected from solutions and declarations.
		BasicType* policy = nullptr;

	public:
		
		//bool isDefInput = false;
		//Right here in qualified type, I can put a set of bits that I can use to relay some ideas of what I can turn it into.


		operator BasicType* ()
		{
			return policy;
		}
		operator const BasicType* () const
		{
			return policy;
		}


		BasicType* operator->()
		{
			return policy;
		}
		const BasicType* operator->() const
		{
			return policy;
		}

		operator bool() const
		{
			return policy;
		}

		auto operator <=>(const QualifiedType&) const = default;

		ConvertResult IsQualified(const QualifiedType& other, ConversionFlag flags, Conversion** out) const;



		//TODO: IsCovertToQualfied needs to hold an IType to see if it has permission to this conversion. How I'd do that, is kinda hard.
		ConvertResult IsConvertToQualified(const QualifiedType& other, BasicType* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const;
	};
}