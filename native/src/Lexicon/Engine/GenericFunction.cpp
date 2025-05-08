#include "Lexicon/Engine/GenericFunction.h"

#include "Lexicon/Function.h"
#include "Lexicon/Engine/SpecialFunction.h"

namespace LEX
{


	IFunction* GenericFunction::CheckFunction(ITemplatePart* args)
	{
		return dynamic_cast<IFunction*>(ObtainSpecial(args));
	}

	Function* GenericFunction::GetFunction(ITemplateBody* args)
	{
		return ObtainBody(args)->ToResolved<Function>();
	}

	std::unique_ptr<SpecialBase> GenericFunction::CreateSpecial(ITemplatePart* args)
	{
		return std::unique_ptr<SpecialBase>{ new SpecialFunction{ this, args } };
	}

}