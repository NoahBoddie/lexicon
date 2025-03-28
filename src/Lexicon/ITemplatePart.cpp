#include "Lexicon/ITemplatePart.h"

#include "Lexicon/GenericArray.h"

namespace LEX
{
	GenericArray ITemplatePart::SpecializeTemplate(ITemplatePart* args)
	{
		//If no args, issue probably.

		auto size = args->GetSize();

		GenericArray result{ args->GetClient(), size };

		for (int x = 0; x < size; x++)
		{
			auto type = args->GetPartArgument(x);

			result._types[x] = type->CheckTypePolicy(args);
		}

		return result;
	}

}