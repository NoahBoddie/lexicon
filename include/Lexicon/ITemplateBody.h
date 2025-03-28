#pragma once

#include "Lexicon/ITemplatePart.h"

//*src
#include "Lexicon/AbstractTypePolicy.h"

namespace LEX
{

	struct AbstractTypePolicy;

	struct ITemplateBody : public ITemplatePart
	{

		ITypePolicy* GetPartArgument(size_t i) const override
		{
			return GetBodyArgument(i);
		}

		//TODO: GetBodyArgument rarely comes up anymore. As long as it has the promise of resolution, I think that's literally all I need out of it.
		virtual AbstractTypePolicy* GetBodyArgument(size_t i) const = 0;


		ITemplateBody* TryPromoteTemplate() override
		{
			return this;
		}
	};
}