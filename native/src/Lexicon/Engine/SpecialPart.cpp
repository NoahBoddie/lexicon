#include "Lexicon/Engine/SpecialPart.h"

#include "Lexicon/Engine/GenericBase.h"

namespace LEX
{

	SpecialBase* SpecialPart::ObtainSpec(GenericBase* ask, ITemplatePart* args)
	{
		//Purify arguments here, then send those args. Since this is par

		auto _array = MakeGenericArray(ask, args);

		return GetGeneric()->ObtainSpecial(ask, _array.get());
	}


	//This is still unhandled perhaps?
	SpecialBody* SpecialPart::ObtainBody(ITemplateBody* args)
	{
		//Purify arguments here, then send those args. Since this is par

		auto _array = MakeGenericArray(args);

		return GetGeneric()->ObtainBody(_array.get());
	}
}