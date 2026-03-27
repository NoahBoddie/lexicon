#include "Lexicon/Engine/GenericBase.h"

#include "Lexicon/Engine/Environment.h"

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

		if (auto temp = args->TryResolve(); temp)
			return ObtainBody(temp);
		else
			return ObtainPart(args);
	}

	bool GenericBase::TemplateMatches(ITemplatePart* args)
	{
		//This expects a completed template btw.

		auto size = _templates.size();



		if (args->GetSize() != size)
			return false;

		for (int i = 0; i < size; i++)
		{
			TemplateType& param = _templates[i];

			//This would be the thing it should be trying to turn into
			ITypeInfo* _param = &param;

			ITypeInfo* arg = args->GetPartArgument(i);

			Element* element = dynamic_cast<Element*>(this);

			ITypeInfo* scope = element->FetchEnvironment()->As<ITypeInfo>();
			
			if constexpr (0)
			{//For now it accepts all, so no real reason to do this.
				auto conv = arg->IsConvertibleTo(_param, scope, nullptr);

				if (conv <= ConversionEnum::Failure) {
					return false;
				}
			}
		}

		return true;
	}

}