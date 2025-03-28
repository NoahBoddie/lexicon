#include "Lexicon/Engine/SpecialBase.h"

#include "Lexicon/GenericArray.h"
#include "Lexicon/Engine/GenericBase.h"

namespace LEX
{
	SpecialBase* SpecialBase::ObtainSpecial(GenericBase* ask, ITemplatePart* args)
	{
		if (IsResolved() == true) {
			return this;
		}

		auto array = SpecializeTemplate(ask, args);

		return GetGeneric()->ObtainSpecial(ask, array);
	}
}