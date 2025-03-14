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

		QualifiedType& operator=(const Qualifier& other) { __super::operator=(other); return *this; }


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

		ConvertResult IsQualified(const QualifiedType& other, ConversionFlag flags, Conversion** out) const
		{
			auto result = ConvertResult::Exact;


			//Reference section
			bool init = flags & ConversionFlag::Initialize;
			bool param = flags & ConversionFlag::Parameter;
			bool ret = flags & ConversionFlag::Return;
			if (init || param || ret)
			{
				if (out && IsReference(false) == true) {
					//No conversions allowed with references
					*out = nullptr;
				}


				//We only care about references if they're being initialized
				Reference refl = reference;
				Reference refr = other.reference;

				//While maybe ref is technically a reference, it needs to be promoted first
				bool not_ref = refr == Reference::Temp || refr ==  Reference::Maybe;
				bool equals = refl == refr;

				bool ref_check = true;

				switch (refl)
				{
				case Reference::Global://For global it must be equal, no exceptions
					if (!equals && refr != Reference::Static)
						return ConvertResult::QualError5;
					break;

				case Reference::Scoped:
				case Reference::Local:
					if (not_ref)
						return ConvertResult::QualError6;
					
					if (!param)//If this is a param, anything goes accept for maybe ref stuff.
					{
						if (ret && equals)
							return ConvertResult::QualError7;

						if (init && refr < refl)
							return ConvertResult::QualError8;
					}
					break;

				case Reference::Static://These 2 literally should never be a return type, and assign wise, this has no issues.
				case Reference::Auto:
				case Reference::Maybe:
				case Reference::Temp://Is this immutable?
					//Anything can go into these, there's no restrictions
					break;

				case Reference::Generic:
					if (refr == Reference::Temp) {
						return ConvertResult::QualError5;//I have no idea if this is the right one, I just don't much care right now.
					}
						
					break;
				}


				if (IsReference(true) && other.IsReferential() && !equals) {
					result = ConvertResult::RefConvert;
				}

				//Auto cannot be accepted by anything at all
			}
			
			



			//This does a conversion but only on the qualifiers.

			//auto comp = qualifiers & other.qualifiers;

			//These in essence did a switch. need to rename to To and from.
			Constness l_comp = constness;
			Constness r_comp = other.constness;

			if (l_comp != r_comp) {

				//this  first bit makes no sense, if the left side is const anything can still go into it (as long as we are initializing).
				if (1 != 1) {
					//if (l_comp == Qualifier::Const)
					if (r_comp == Constness::Const)
					{
						//If left is const, it must be a class type.
						if (IsDataTypeRef(policy->FetchDataType()) == false)
							return ConvertResult::QualError1;
					}
				}
				//This doesn't seem to work, but it will be hit by a forcible transfer issue.
				//if (r_comp == Qualifier::Const)
				if (l_comp == Constness::Const)
				{
					//If right is const, it must be a value.
					if (IsDataTypeVal(other.policy->FetchDataType()) == false)
						return ConvertResult::QualError2;
				}

				if (result == ConvertResult::Exact)
					result = ConvertResult::ConstConvert;
			}

			//Simple for now.
			return result;
		}



		//TODO: IsCovertToQualfied needs to hold an ITypePolicy to see if it has permission to this conversion. How I'd do that, is kinda hard.
		ConvertResult IsConvertToQualified(const QualifiedType& other, ITypePolicy* scope, Conversion* out = nullptr, ConversionFlag flags = ConversionFlag::None) const
		{

			//TODO: ConvertQualified needs 2 rules, read below
			// first if ref types are used, no conversions are allowed (return to nullptr). Also that Conversion on maybe refs should present a warning
			// due to not actually using a reference if a reference was desired.




			if (auto result = IsQualified(other, flags, &out); result != ConvertResult::Exact)
				return result;

			//Simple for now.
			return policy->IsConvertibleTo(other.policy, scope, out, flags);


			//TODO: ConverstResult needs to be comprised of more types.  \/
			// Namely, something to tell what error it should give, as well as a function that takes the qualified types that failed

			ConvertResult qual_result = IsQualified(other, flags, &out);

			if (qual_result <= ConvertResult::Failure)
				return qual_result;

			if (auto type_result = policy->IsConvertibleTo(other.policy, scope, out, flags); type_result != ConvertResult::Exact)
				return type_result;

			return qual_result;



			return policy->IsConvertibleTo(other.policy, scope, out, flags);
		}
	};
}