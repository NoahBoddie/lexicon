#include "Lexicon/Engine/GenericBase.h"

namespace LEX
{
	SpecialPart* GenericBase::FindPart(GenericBase* tar, ITemplatePart* args)
	{
		for (auto& spec : incomplete)
		{
			//First compare target, then compare args.

			if (spec->GetGeneric() == tar && spec->TemplateMatches(args) == true)
				return spec.get();
		}

		return nullptr;
	}

	SpecialBody* GenericBase::FindBody(ITemplateBody* args)
	{
		for (auto& spec : complete)
		{
			//report::critical("This shant be used go back");
			if (spec->TemplateMatches(args) == true)
				return spec.get();
		}

		return nullptr;
	}
	void GenericBase::SpecializeParts(ITemplateBody* args)
	{
		for (auto& part : incomplete) {
			report::info("thing {}", (uintptr_t)part.get());
			part->ObtainBody(args);
		}
	}

	SpecialBase* GenericBase::ObtainSpecial(GenericBase* client, ITemplatePart* args)
	{
		if (TemplateMatches(args) == false)
		{
			report::fault::error("cant handle args");
		}

		if (auto temp = args->TryPromoteTemplate(); temp)
			return ObtainBody(temp);
		else
			return ObtainPart(client, args);
	}
}