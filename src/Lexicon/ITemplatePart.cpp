#include "Lexicon/ITemplatePart.h"

#include "Lexicon/GenericArray.h"

namespace LEX
{
	GenericArray ITemplatePart::SpecializeTemplate(GenericBase* client, ITemplatePart* args)
	{
		//If no args, issue probably.

		auto size = args->GetSize();

		//auto client = args->GetClient();

		GenericArray result{ client, size };

		for (int x = 0; x < size; x++)
		{
			auto type = args->GetPartArgument(x);

			result._types[x] = type->CheckTypePolicy(client, args);
		}

		return result;
	}

}