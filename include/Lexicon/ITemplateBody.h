#pragma once

#include "Lexicon/ITemplatePart.h"

//*src
#include "Lexicon/TypeInfo.h"

namespace LEX
{

	struct TypeInfo;

	struct ITemplateBody : public ITemplatePart
	{

		//TODO: GetBodyArgument rarely comes up anymore. As long as it has the promise of resolution, I think that's literally all I need out of it.
		virtual TypeInfo* GetBodyArgument(size_t i) const = 0;


		ITemplateBody* TryResolve() override
		{
			return this;
		}
	};
}