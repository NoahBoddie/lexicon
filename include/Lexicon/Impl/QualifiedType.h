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


	struct QualifiedType
	{
		//Within the compiling process, this is more often than not what should be used.
		//TODO: QualifiedType replaces param input.

		QualifiedType() = default;

		QualifiedType(std::nullptr_t) {}

		explicit QualifiedType(ITypePolicy* p) : policy{ p } {}
		QualifiedType(ITypePolicy* p, Qualifier q) : policy{ p }, flags{ q } {}


	//private://Temporary to sus out where policy is expected from solutions and declarations.
		ITypePolicy* policy = nullptr;

	public:

		Qualifier flags = Qualifier::None;
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
		//TODO: IsCovertToQualfied needs to hold an ITypePolicy to see if it has permission to this conversion. How I'd do that, is kinda hard.
		ConvertResult IsConvertToQualified(QualifiedType& other, ITypePolicy* scope, Conversion* out = nullptr, bool is_expl = false) const
		{

			//auto comp = flags & other.flags;

			//These in essence did a switch. need to rename to To and from.
			auto l_comp = flags & Qualifier::Const;
			auto r_comp = other.flags & Qualifier::Const;

			if (l_comp != r_comp){
				
				//this  first bit makes no sense, if the left side is const anything can still go into it (as long as we are initializing).
				if (1 != 1) {
					//if (l_comp == Qualifier::Const)
					if (r_comp == Qualifier::Const)
					{
						//If left is const, it must be a class type.
						if (IsDataTypeRef(policy->FetchDataType()) == false)
							return ConvertResult::QualError1;
					}
				}
				//This doesn't seem to work, but it will be hit by a forcible transfer issue.
				//if (r_comp == Qualifier::Const)
				if (l_comp == Qualifier::Const)
				{
					//If right is const, it must be a value.
					if (IsDataTypeVal(other.policy->FetchDataType()) == false)
						return ConvertResult::QualError2;
				}
			}

			//Simple for now.
			return policy->IsConvertibleTo(other.policy, scope, out, is_expl ? ConversionType::Implicit : ConversionType::Implicit);
		}
		
		ConvertResult IsConvertToQualified(QualifiedType&& other, ITypePolicy* scope, Conversion* out = nullptr, bool is_expl = false) const
		{
			//This is the over version of can convert. Accounts for qualifiers and such. Speaking of,
			// such a thing needs to be able to use the search functions too should it not?

			//Actually, I'm realizing a last thing.


			return IsConvertToQualified(other, scope, out, is_expl);
		}



		int CompareType(OverloadCode& left, OverloadCode& right, QualifiedType& left_type, QualifiedType& right_type);

		int CompareType(QualifiedType& a_lhs, QualifiedType& a_rhs);
	};
}