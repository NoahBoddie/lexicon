#pragma once

#include "Lexicon/ITemplatePart.h"

//*src
#include BASIC_NAME(Type)

namespace LEX
{

	struct Type;

	struct ITemplateBody : public ITemplatePart
	{

		//TODO: GetBodyArgument rarely comes up anymore. As long as it has the promise of resolution, I think that's literally all I need out of it.
		virtual Type* GetBodyArgument(size_t i) const = 0;


		ITemplateBody* TryPromoteTemplate() override
		{
			return this;
		}
	};
}