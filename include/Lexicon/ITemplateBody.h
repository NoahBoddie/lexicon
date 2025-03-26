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


		virtual AbstractTypePolicy* GetBodyArgument(size_t i) const = 0;


		ITemplateBody* TryPromoteTemplate() override
		{
			return this;
		}
	};
}