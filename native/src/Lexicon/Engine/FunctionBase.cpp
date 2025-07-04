#include "Lexicon/Engine/FunctionBase.h"

#include "Lexicon/Engine/Expression.h"
#include "Lexicon/Engine/Environment.h"
#include "Lexicon/Engine/Declaration.h"
#include "Lexicon/Engine/RoutineCompiler.h"
#include "Lexicon/Engine/TypeBase.h"

#include "Lexicon/ProcedureData.h"

#include "Lexicon/Engine/Runtime.h"

#include "Lexicon/Engine/parse_strings.h"

namespace LEX
{



    void FunctionBase::LoadFromRecord(SyntaxRecord& target)
    {
        /*
        'GetActorValue' <Expression: Function (col: 38/ line: 2>
            'type' <Expression: Header>
                'ActorValueStruct' <Expression: Typename (col: 21/ line: 2>
                    'ActorValue' <Expression: Scriptname (col: 9/ line: 2>
            'params' <Expression: Header>
                'a1' <Expression: VarName (col: 58/ line: 2>
                    'type' <Expression: Header>
                        'float' <Expression: Typename (col: 52/ line: 2>
                'a2' <Expression: VarName (col: 68/ line: 2>
                    'type' <Expression: Header>
                        'float' <Expression: Typename (col: 62/ line: 2>
                'a3' <Expression: VarName (col: 86/ line: 2>
                    'type' <Expression: Header>
                        'type' <Expression: Typename (col: 81/ line: 2>
                            'commons' <Expression: Scriptname (col: 72/ line: 2>

        //*/

        if (target.SYNTAX().type != SyntaxType::Function) {
            report::compile::critical("Not a function, cannot load.");
        }

        /*
        //This makes it search twice, but EH, I don't think I care much. I might seperate declarations from specifiers or make an option
        // to ignore later. Ain't interface so it's ripe to burn
        SyntaxRecord* head_rec = target.FindChild(parse_strings::header);

        if (!head_rec)
            report::compile::critical("No record named header.");

        
        SyntaxRecord& decl_spec = *head_rec->FindChild(parse_strings::declare_specifier);
        auto spec = GetSpecifiersFromStrings(decl_spec);
        if (spec & SpecifierFlag::External)
        {
            _procedure = nullptr;
            procedureData = -1;
        }

        //*/

        _name = target.GetTag();
	}

    void FunctionBase::SetReturnType(QualifiedType type)
    {
        _returnType = type;
    }



    LinkResult FunctionBase::OnLink(LinkFlag flags)
    {

        SyntaxRecord& target = *GetSyntaxTree();

        //I would cycle_switch but seems a bit shit.
        switch (flags)
        {
        case LinkFlag::Declaration:
        {
            //Environment* environment = GetEnvironment();

            SyntaxRecord* head_rec = target.FindChild(parse_strings::header);

            if (!head_rec)
				report::compile::critical("No record named header.");
            //LINK_AFTER

            //If function is seen as being static, it should not allow for 

            Declaration header = Declaration::Create(*head_rec, this, Refness::Local, HeaderFlag::Mutable);


            //Declaration header{ *head_rec, this, Refness::Local };
            //if (header.Matches(DeclareMatches::Constness) == false) {
			//	report::compile::critical("Either unexpected qualifiers/specifiers or no type when type expected.");
            //}

            if (header.SpecifierFlags() & SpecifierFlag::External)
            {
                _procedure = nullptr;
                procedureData = -1;
            }
            else
            {
                _routine.name = GetName().data();
            }

            //ITypeInfo* policy = environment->TEMPSearchType(target.FindChild("type")->GetFront().GetTag());

            QualifiedType type = QualifiedType{ header };

            //GENERIC_SPACE

            SetReturnType(type);


            //STATIC_CHECK

            bool method = false;

            if (auto extend = target.FindChild(parse_strings::extends); extend)
            {
                method = true;

                auto& tag = extend->GetFront().GetTag();

                auto target_type = GetPolicyFromSpecifiers(*extend, this);
                

                if (!target_type) {
                    report::link::error("No type found with the name '{}' (tag not accurate anymore)", tag);
                }
                


                //Qualifiers like const are put here depending on if the function is const. 
                // We don't have those post declarations yet.
				
                //auto& param = parameters.emplace_back(QualifiedType{ _targetType }, parse_strings::this_word, 0);
                //report::fault::trace("Adding {} to {}, type {}", param.GetFieldName(), GetName(), param.GetType()->GetName());
                
                //Include things like whether this is

                
                _thisInfo = std::make_unique<ParameterInfo>(QualifiedType{ target_type }, parse_strings::this_word, 0);
                target_type->SetSelfQualifiers(_thisInfo->qualifiers);
            }


            for (int64_t i = 0; auto & node : target.FindChild(parse_strings::parameters)->children())
            {
                SyntaxRecord* node_head = node.FindChild(parse_strings::header);

                if (!node_head)
					report::compile::critical("No record named header.");

                //Declaration header{ *node_head, this, Refness::Scoped, Refness::Auto };
                Declaration header = Declaration::Create(*node_head, this, Refness::Scoped, Refness::Auto, 
                    HeaderFlag::MemberSpecifiers | HeaderFlag::FunctionSpecifiers);


                //Unlike the return type, clearly parameters cannot be static, that's a compiling error.
                //if (header.Matches(DeclareMatches::Constness | DeclareMatches::Refness) == false) {
				//	report::compile::critical("Either unexpected qualifiers/specifiers or no type when type expected.");
                //}

                QualifiedType type = QualifiedType{ header };

                //auto& tag = node.FindChild("type")->GetFront().GetTag();

                //ITypeInfo* policy = environment->TEMPSearchType(node.FindChild("type")->GetFront().GetTag());

                //if (!policy) {
                //    report::link::error("Parameter type '{}' couldn't be found", tag);
                //}

                //GENERIC_SPACE

                //auto& param = parameters.emplace_back(type, node.GetTag(), method + i++);
                auto& param = parameters.emplace_back(type, node.GetTag(), GetParamCount());
                
                assert(param.GetType());
            }

            break;
        }

        case LinkFlag::Definition:
        {
            if (!procedureData || procedureData != -1) {
                if (target.FindChild(parse_strings::code) == nullptr)
                    report::compile::error("Function '{}' doesn't have a body", GetName());

                if (RoutineCompiler::Compile(_routine, target, this) == false){
                    return LinkResult::Failure;
                }
            }
            break;
        }

        case LinkFlag::Final:
        {
            if (_procedure.has_value() && _procedure.value() == nullptr) {
                report::link::failure("Function '{}' did not register a procedure.");
                return LinkResult::Failure;
            }
            break;
        }
        break;

        default:
            //Invalid linkage
            return LinkResult::Failure;

        }


        return LinkResult::Success;
    }

    void FunctionBase::OnLinkComplete()
    {
        //I want a way to fire this off and still have links complete. Gonna take some thought though.
        //if (GetValid() == false) {
        //    report::link::warn("Function '{}' didn't register a procedure at the end of linking.", GetName());
        //}
    }

    bool FunctionBase::GetValid() const
    {
        return !_procedure.has_value() || _procedure.value();
    }

    LinkFlag FunctionBase::GetLinkFlags() 
    {
        //return LinkFlag::None;
        //Needs to handle linking once when declaration happens 
        return LinkFlag::Declaration | LinkFlag::Definition;
    }


    RuntimeVariable FunctionBase::BasicExecute(Function* self, ITemplateBody* body, std::span<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def)
    {
        if (IsValid() == false) {
            report::log("Function '{}' is not valid. Maybe say error later.", IssueType::Apply, def ? IssueLevel::Failure : IssueLevel::Error, GetName());

            //disable this warning.

            return *def;
        }

        return __super::BasicExecute(self, body, args, caller, def);
    }
    



}