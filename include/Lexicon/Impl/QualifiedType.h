#pragma once


#include "Qualifier.h"
//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"


//*src
#include "ITypePolicy.h"
namespace LEX
{
	struct ITypePolicy;

	struct QualifiedType
	{
		//Within the compiling process, this is more often than not what should be used.
		//TODO: QualifiedType replaces param input.


		ITypePolicy* policy = nullptr;
		Qualifier qualifiers = Qualifier::None;
		bool isDefInput = false;
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


		bool CanConvertToQualified(QualifiedType&& other, Conversion* out = nullptr, bool is_expl = false) const
		{
			//This is the over version of can convert. Accounts for qualifiers and such. Speaking of,
			// such a thing needs to be able to use the search functions too should it not?

			//Actually, I'm realizing a last thing.


			return false;
		}
	};
}