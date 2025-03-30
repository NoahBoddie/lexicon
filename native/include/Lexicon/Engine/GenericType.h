#pragma once

#include "Lexicon/Engine/TypeBase.h"
#include "Lexicon/Engine/GenericBase.h"

namespace LEX
{
	struct SpecialBase;

	struct GenericType : public GenericTypeBase, public GenericBase
	{
		ISpecializable* GetSpecializable() override { return this; }

		bool IsResolved() const override { return false; }

		AbstractType* CheckTypePolicy(ITemplatePart* args) override;

		Type* GetTypePolicy(ITemplateBody* args) override;

		std::unique_ptr<SpecialBase> CreateSpecial(ITemplatePart* args) override;
	};


}