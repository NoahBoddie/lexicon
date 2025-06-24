#include "Lexicon/Engine/TemplateType.h"

#include "Lexicon/ITemplatePart.h"
#include "Lexicon/ITemplateBody.h"


namespace LEX
{
	ITypeInfo* TemplateType::CheckTypePolicy(ITemplatePart* args)
	{
		auto part = args->GetPartArgument(index);

		return part;
	}

	TypeInfo* TemplateType::GetTypePolicy(ITemplateBody* args)// override
	{
		auto body = args->GetBodyArgument(index);

		return body;
	}

}