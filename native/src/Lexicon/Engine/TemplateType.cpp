#include "Lexicon/Engine/TemplateType.h"

#include "Lexicon/ITemplatePart.h"
#include "Lexicon/ITemplateBody.h"


namespace LEX
{
	ITypePolicy* TemplateType::CheckTypePolicy(GenericBase* ask, ITemplatePart* args)
	{
		auto part = args->GetPartArgument(index);

		return part;
	}

	AbstractTypePolicy* TemplateType::GetTypePolicy(ITemplateBody* args)// override
	{
		auto body = args->GetBodyArgument(index);

		return body;
	}

}