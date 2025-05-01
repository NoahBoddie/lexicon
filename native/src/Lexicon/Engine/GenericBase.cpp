#include "Lexicon/Engine/GenericBase.h"

namespace LEX
{
	SpecialBase* GenericBase::FindPart(GenericBase* tar, ITemplatePart* args)
	{
		for (auto& spec : incomplete)
		{
			//First compare target, then compare args.

			if (spec->GetGeneric() == tar && spec->TemplateMatches(args) == true)
				return spec.get();
		}

		return nullptr;
	}

	SpecialBase* GenericBase::FindBody(ITemplateBody* args)
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
			part->ObtainSpecial(args);
		}
	}

	SpecialBase* GenericBase::ObtainSpecial(ITemplatePart* args)
	{
		//TODO: Currently, the problem is we can't send something like a 3 templated part for a 5 templated function.
		// Currently

		if (TemplateMatches(args) == false) {
			report::fault::error("cant handle args");
		}

		//TODO: Merge ObtainBody/ObtainPart into this, there's no specialization for it, so no need.

		if (auto temp = args->TryPromoteTemplate(); temp)
			return ObtainBody(temp);
		else
			return ObtainPart(args);
	}
}