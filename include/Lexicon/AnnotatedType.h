#pragma once

#include "Qualifier.h"
//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"


//*src
#include BASIC_NAME(Type)

namespace LEX
{
	struct AnnotatedType : public Qualifier
	{
		//Within the compiling process, this is more often than not what should be used.
		//TODO: QualifiedType replaces param input.

		AnnotatedType() = default;

		AnnotatedType(std::nullptr_t) {}

		explicit AnnotatedType(BasicType* p) : policy{ p } {}
		AnnotatedType(BasicType* p, Qualifier q) : policy{ p }, Qualifier{ q } {}

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

		auto operator <=>(const AnnotatedType&) const = default;

	};
}