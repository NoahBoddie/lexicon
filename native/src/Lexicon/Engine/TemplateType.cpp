#include "Lexicon/Engine/TemplateType.h"

#include "Lexicon/ITemplatePart.h"
#include "Lexicon/ITemplateBody.h"

#include "Lexicon/Engine/GenericBase.h"

namespace LEX
{
	ITypeInfo* TemplateType::CheckTypeInfo(ITemplatePart* args)
	{
		auto part = args->GetPartArgument(index);

		return part;
	}

	TypeInfo* TemplateType::GetTypeInfo(ITemplateBody* args)// override
	{
		auto body = args->GetBodyArgument(index);

		return body;
	}
	
	ISpecializable* TemplateType::GetSpecializable() 
	{
		return _owner; 
	}

	const Component* TemplateType::AsComponent() const
	{ 
		return _owner ? _owner->AsComponent() : nullptr; 
	}

}