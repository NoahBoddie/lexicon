#include "Lexicon/Engine/GenericType.h"


#include "Lexicon/Engine/SpecialType.h"

namespace LEX
{

	ITypePolicy* GenericType::CheckTypePolicy(ITemplatePart* args)
	{
		return dynamic_cast<ITypePolicy*>(ObtainSpecial(args));
	}
	AbstractTypePolicy* GenericType::GetTypePolicy(ITemplateBody* args)
	{
		return dynamic_cast<AbstractTypePolicy*>(ObtainBody(args));
	}

	/*
	std::unique_ptr<SpecialBase> GenericType::CreatePart(ITemplatePart* args)
	{

		return std::make_unique<SpecialType>(this, args);
	}

	std::unique_ptr<SpecialBase> GenericType::CreateBody(ITemplateBody* args)
	{
		return std::make_unique<SpecialType>(this, args);
	}
	//*/

	std::unique_ptr<SpecialBase> GenericType::CreateSpecial(ITemplatePart* args)
	{
		return std::unique_ptr<SpecialBase>{ new SpecialType {this, args} };
	}

}