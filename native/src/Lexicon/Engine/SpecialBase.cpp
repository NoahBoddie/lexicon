#include "Lexicon/Engine/SpecialBase.h"

#include "Lexicon/GenericArray.h"
#include "Lexicon/Engine/GenericBase.h"

namespace LEX
{
	SpecialBase* SpecialBase::ObtainSpecial(ITemplatePart* args)
	{
		if (IsResolved() == true) {
			return this;
		}

		auto array = SpecializeTemplate(args);

		return GetGeneric()->ObtainSpecial(array);
	}
}