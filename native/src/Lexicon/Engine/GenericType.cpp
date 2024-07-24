#include "Lexicon/Engine/GenericType.h"


#include "Lexicon/Engine/SpecialTypePart.h"
#include "Lexicon/Engine/SpecialTypeBody.h"

namespace LEX
{

	ITypePolicy* GenericType::CheckTypePolicy(GenericBase* base, ITemplatePart* args)
	{
		return dynamic_cast<ITypePolicy*>(ObtainSpecial(base, args));
	}
	AbstractTypePolicy* GenericType::GetTypePolicy(ITemplateBody* args)
	{
		return dynamic_cast<AbstractTypePolicy*>(ObtainBody(args));
	}

	std::unique_ptr<SpecialPart> GenericType::CreatePart(ITemplatePart* args)
	{

		return std::make_unique<SpecialTypePart>(this, args);
	}

	std::unique_ptr<SpecialBody> GenericType::CreateBody(ITemplateBody* args)
	{
		return std::make_unique<SpecialTypeBody>(this, args);
	}

}