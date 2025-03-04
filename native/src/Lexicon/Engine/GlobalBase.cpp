#include "Lexicon/Engine/GlobalBase.h"
#include "Lexicon/Engine/Expression.h"
#include "Lexicon/Engine/parse_strings.h"
#include "Lexicon/Engine/Declaration.h"
#include "Lexicon/Engine/RoutineCompiler.h"

#include "Lexicon/Engine/SyntaxRecord.h"

#include "Lexicon/Engine/ConcreteGlobal.h"
namespace LEX
{

	void GlobalBase::LoadFromRecord(SyntaxRecord& target)
	{
		if (target.SYNTAX().type != SyntaxType::Variable) {
			report::compile::critical("Not a function, cannot load.");
		}

		_name = target.GetTag();
	}

	void GlobalBase::OnAttach()
	{
        //GetSyntaxTree()->Note("New Global {}", GetName());
	}

	LinkResult GlobalBase::OnLink(LinkFlag flags)
	{

        SyntaxRecord& target = *GetSyntaxTree();

        //I would cycle_switch but seems a bit shit.
        switch (flags)
        {
        case LinkFlag::Declaration:
        {

            Environment* environment = GetEnvironment();

            if (!environment) {
                report::link::error("environ issues cont.");
            }

            SyntaxRecord* head_rec = target.FindChild(parse_strings::header);

            if (!head_rec)
                report::compile::critical("No record named header.");
            //LINK_AFTER

            Declaration header{ *head_rec, environment };
            
            
            constexpr auto spec = ~(SpecifierFlag::External | SpecifierFlag::Virtual) & SpecifierFlag::All;
            

            if (header.Matches(DeclareMatches::Constness, spec) == false) {
                report::compile::critical("Either unexpected qualifiers/specifiers or missing type.");
            }

            //if (auto filter = header.Filter(qual, spec); filter) {
            //    report::compile::critical("Either unexpected qualifiers/specifiers or missing type.");
            //}



            _declared = header;

            
            break;
        }

        case LinkFlag::Definition:
        {
            //This should simply set the default value.
            Revert(true);
            
            //this method of execution will need to change to something more up to date at some point.
            // For now it works.
            FunctionData tempData = FunctionData{};
            tempData._name = _name;
            tempData._returnType = _declared;
            
            if (auto def = target.FindChild(parse_strings::def_expression); def)
            {
                _init = std::make_unique<RoutineBase>();


                if (RoutineCompiler::Compile(*_init, def->GetFront(), &tempData, GetEnvironment()) == false)
                {
                    FlagAsInvalid();
                }
                //*_init = RoutineCompiler::Compile(def->GetFront(), &tempData, GetEnvironment());
                 
            }

            break;
        }

        case LinkFlag::External://This should be final, but I haven't gotten around to it.
        {
            

            //Only will attempt to revert to init if init has already been done.
            if (_init) {
                Revert(false);
                //if (auto self = As<ConcreteGlobal>())
                //    report::compile::debug("Global {} created with the value {}", GetName(), self->PrintString());
            }
        }
        break;

        default:
            //Invalid linkage
            return LinkResult::Failure;

        }


        return LinkResult::Success;
	}

	LinkFlag GlobalBase::GetLinkFlags()
	{
		return LinkFlag::Declaration | LinkFlag::Definition | LinkFlag::External;
	}

}