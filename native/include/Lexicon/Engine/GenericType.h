#pragma once

#include "Lexicon/Engine/PolicyBase.h"
#include "Lexicon/Engine/GenericBase.h"

namespace LEX
{
	struct SpecialBase;
	struct SpecialBase;

	struct GenericType : public PolicyBase, public GenericBase
	{
		//ITypePolicy* GetTypePolicy()


		ITypePolicy* CheckTypePolicy(GenericBase* base, ITemplatePart* args) override;

		AbstractTypePolicy* GetTypePolicy(ITemplateBody* args) override;

		std::unique_ptr<SpecialBase> CreatePart(ITemplatePart* args) override;

		std::unique_ptr<SpecialBase> CreateBody(ITemplateBody* args) override;

		std::unique_ptr<SpecialBase> CreateSpecial(ITemplatePart* args) override;
	};


}