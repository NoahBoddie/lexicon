#pragma once

#include "Qualifier.h"
//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"


//*src
#include "Lexicon/TypeInfo.h"

namespace LEX
{
	struct AnnotatedType : public Qualifier
	{
		//Within the compiling process, this is more often than not what should be used.
		//TODO: QualifiedType replaces param input.

		AnnotatedType() = default;

		AnnotatedType(std::nullptr_t) {}

		explicit AnnotatedType(ITypeInfo* p) : policy{ p } {}
		AnnotatedType(ITypeInfo* p, Qualifier q) : policy{ p }, Qualifier{ q } {}

		//TODO:Remove this plz it's confusing looking
		using Qualifier::operator=;//(const Qualifier& other) { __super::operator=(other); return *this; }


		//private://Temporary to sus out where policy is expected from solutions and declarations.
		ITypeInfo* policy = nullptr;

	public:

		//bool isDefInput = false;
		//Right here in qualified type, I can put a set of bits that I can use to relay some ideas of what I can turn it into.


		operator ITypeInfo* ()
		{
			return policy;
		}
		operator const ITypeInfo* () const
		{
			return policy;
		}


		ITypeInfo* operator->()
		{
			return policy;
		}
		const ITypeInfo* operator->() const
		{
			return policy;
		}

		operator bool() const
		{
			return policy;
		}

		template<typename T>
		T QualifySpecial(this const T& self, ITemplatePart* part)
		{
			return T{ self.policy->CheckTypePolicy(part), self };
		}

		auto operator <=>(const AnnotatedType&) const = default;

	};
}