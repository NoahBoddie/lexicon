#pragma once

#include "Lexicon/Engine/PolicyBase.h"
#include "Lexicon/Engine/GenericBase.h"

namespace LEX
{
	struct SpecialPart;
	struct SpecialBody;

	struct GenericType : public PolicyBase, public GenericBase
	{
		//ITypePolicy* GetTypePolicy()


		ITypePolicy* CheckTypePolicy(GenericBase* base, ITemplatePart* args) override;

		AbstractTypePolicy* GetTypePolicy(ITemplateBody* args) override;

		std::unique_ptr<SpecialPart> CreatePart(ITemplatePart* args) override;

		std::unique_ptr<SpecialBody> CreateBody(ITemplateBody* args) override;

	};


}