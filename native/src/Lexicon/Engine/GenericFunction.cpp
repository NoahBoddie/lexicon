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




    LinkResult GenericFunction::OnLink(LinkFlag flags)
    {

        SyntaxRecord& generic = *GetSyntaxTree()->FindChild(parse_strings::generic);

        //I would cycle_switch but seems a bit shit.
        switch (flags)
        {
        case LinkFlag::Declaration: {
            for (auto& temp : generic.children()) {
                AddTemplate(temp.GetView());
            }
        }
        break;


        }


        return __super::OnLink(flags);
    }


}