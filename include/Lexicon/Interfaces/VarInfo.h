#pragma once
#include "Lexicon/Interfaces/Info.h"
#include "Lexicon/Qualifier.h"

#ifdef LEX_SOURCE
#include "Lexicon/Engine/QualifiedType.h"
#endif

namespace LEX
{
	namespace Version
	{
		namespace _1
		{
			struct M_INTERFACE_VERSION(VarInfo)
			{
				virtual ITypeInfo* GetType() const = 0;
				virtual Refness GetRefness() = 0;
				virtual Constness GetConstness() = 0;
				virtual QualifierFlag GetQualifierFlags() = 0;
			};
		}

		CURRENT_VERSION(VarInfo, 1);
	}


	struct IMPL_VERSION(VarInfo, Info)
	{
		DEFINE_INFO_OFFSET(InfoType::VarInfo)
	public:


		Qualifier GetQualifiers()
		{
			return Qualifier{ GetRefness(), GetConstness(), GetQualifierFlags()};
		}
#ifdef LEX_SOURCE
		QualifiedType GetQualifiedType()
		{
			QualifiedType result{};

			result.policy = GetType();

			result = GetQualifiers();

			return result;
		}
#endif

		
	};
}