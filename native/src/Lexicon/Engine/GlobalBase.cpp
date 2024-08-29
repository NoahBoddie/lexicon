#include "Lexicon/Engine/GlobalBase.h"
#include "Lexicon/Engine/Expression.h"
#include "Lexicon/Engine/parse_strings.h"
#include "Lexicon/Engine/Declaration.h"
#include "Lexicon/Engine/RoutineCompiler.h"

#include "Lexicon/Engine/SyntaxRecord.h"
namespace LEX
{

	void GlobalBase::LoadFromRecord(Record& target)
	{
		if (target.SYNTAX().type != SyntaxType::Variable) {
			report::compile::critical("Not a function, cannot load.");
		}

		_name = target.GetTag();
	}

	void GlobalBase::OnAttach()
	{
        reinterpret_cast<SyntaxRecord*>(GetSyntaxTree())->Note("New Global {}", GetName());
	}

	LinkResult GlobalBase::OnLink(LinkFlag flags)
	{

        Record& target = *GetSyntaxTree();

        //I would cycle_switch but seems a bit shit.
        switch (flags)
        {
        case LinkFlag::Declaration:
        {

            Environment* environment = GetEnvironment();

            if (!environment) {
                report::link::error("environ issues cont.");
            }

            Record* head_rec = target.FindChild(parse_strings::header);

            if (!head_rec)
                report::compile::critical("No record named header.");
            //LINK_AFTER

            Declaration header{ *head_rec, environment };
            
            
            //This lies, there is no error.
            constexpr auto qual = Qualifier::Constness_;
            constexpr auto spec = ~(Specifier::External | Specifier::Virtual) & Specifier::All;
            

            if (header.Matches(qual, spec) == false) {
                report::compile::critical("Either unexpected qualifiers/specifiers or missing type.");
            }

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

                *_init = RoutineCompiler::Compile(def->GetFront(), &tempData, GetEnvironment());
            }

            break;
        }

        case LinkFlag::External://This should be final, but I haven't gotten around to it.
        {
            //Only will attempt to revert to init if init has already been done.
            if (_init)
                Revert(false);
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