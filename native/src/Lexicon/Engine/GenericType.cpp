#include "Lexicon/Engine/GenericType.h"


#include "Lexicon/Engine/SpecialType.h"

namespace LEX
{

	ITypeInfo* GenericType::CheckTypeInfo(ITemplatePart* args)
	{
		return dynamic_cast<ITypeInfo*>(ObtainSpecial(args));
	}
	TypeInfo* GenericType::GetTypeInfo(ITemplateBody* args)
	{
		return ObtainBody(args)->ToResolved<TypeInfo>();
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