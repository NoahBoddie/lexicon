#include "Lexicon/Engine/GenericType.h"


#include "Lexicon/Engine/SpecialType.h"

namespace LEX
{

	AbstractType* GenericType::CheckTypePolicy(ITemplatePart* args)
	{
		return dynamic_cast<AbstractType*>(ObtainSpecial(args));
	}
	Type* GenericType::GetTypePolicy(ITemplateBody* args)
	{
		return ObtainBody(args)->ToResolved<Type>();
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