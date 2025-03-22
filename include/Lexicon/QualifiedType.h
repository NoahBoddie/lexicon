#pragma once


#include "Qualifier.h"
//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"


//*src
#include "ITypePolicy.h"

namespace LEX
{
	struct ITypePolicy;
	struct OverloadCode;










	//TODO: Merge Qualified Type with it's qualifiers
	struct QualifiedType : public Qualifier
	{
		//Within the compiling process, this is more often than not what should be used.
		//TODO: QualifiedType replaces param input.

		QualifiedType() = default;

		QualifiedType(std::nullptr_t) {}

		explicit QualifiedType(ITypePolicy* p) : policy{ p } {}
		QualifiedType(ITypePolicy* p, Qualifier q) : policy{ p }, Qualifier{ q } {}

		using Qualifier::operator=;//(const Qualifier& other) { __super::operator=(other); return *this; }


	//private://Temporary to sus out where policy is expected from solutions and declarations.
		ITypePolicy* policy = nullptr;

	public:
		
		//bool isDefInput = false;
		//Right here in qualified type, I can put a set of bits that I can use to relay some ideas of what I can turn it into.


		operator ITypePolicy* ()
		{
			return policy;
		}
		operator const ITypePolicy* () const
		{
			return policy;
		}


		ITypePolicy* operator->()
		{
			return policy;
		}
		const ITypePolicy* operator->() const
		{
			return policy;
		}

		operator bool() const
		{
			return policy;
		}

		auto operator <=>(const QualifiedType&) const = default;

		ConvertResult IsQualified(const QualifiedType& other, ConversionFlag flags, Conversion** out) const;



		//TODO: IsCovertToQualfied needs to hold an ITypePolicy to see if it has permission to this conversion. How I'd do that, is kinda hard.
		ConvertResult IsConvertToQualified(const QualifiedType& other, ITypePolicy* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const
		{

			//TODO: ConvertQualified needs 2 rules, read below
			// first if ref types are used, no conversions are allowed (return to nullptr). Also that Conversion on maybe refs should present a warning
			// due to not actually using a reference if a reference was desired.



#ifdef DISABLE_THIS
			if (auto result = IsQualified(other, flags, &out); result != ConvertResult::Exact || out)
				return result;

			//Simple for now.
			return policy->IsConvertibleTo(other.policy, scope, out, flags);
#endif

			//TODO: ConverstResult needs to be comprised of more types.  \/
			// Namely, something to tell what error it should give, as well as a function that takes the qualified types that failed

			ConvertResult qual_result = IsQualified(other, flags, &out);

			if (qual_result <= ConvertResult::Failure || qual_result > ConvertResult::Transformative)
				return qual_result;

			if (auto type_result = policy->IsConvertibleTo(other.policy, scope, out, flags); type_result != ConvertResult::Exact)
				return type_result;

			return qual_result;
		}
	};
}