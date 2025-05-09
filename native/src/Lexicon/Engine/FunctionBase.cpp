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

            Declaration header{ *head_rec, this, Reference::Local };

            if (header.Matches(DeclareMatches::Constness) == false) {
				report::compile::critical("Either unexpected qualifiers/specifiers or no type when type expected.");
            }

            if (header.SpecifierFlags() & SpecifierFlag::External)
            {
                procedureData = -1;
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
            }


            for (int64_t i = 0; auto & node : target.FindChild(parse_strings::parameters)->children())
            {
                SyntaxRecord* node_head = node.FindChild(parse_strings::header);

                if (!node_head)
					report::compile::critical("No record named header.");

                Declaration header{ *node_head, this, Reference::Scoped, Reference::Auto };

                //Unlike the return type, clearly parameters cannot be static, that's a compiling error.
                if (header.Matches(DeclareMatches::Constness | DeclareMatches::Reference) == false) {
					report::compile::critical("Either unexpected qualifiers/specifiers or no type when type expected.");
                }

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
            if (procedureData && !_procedure)
                report::link::error("Function '{}' did not register a procedure.");
            break;
        }
        break;

        default:
            //Invalid linkage
            return LinkResult::Failure;

        }


        return LinkResult::Success;
    }

    LinkFlag FunctionBase::GetLinkFlags() 
    {
        //return LinkFlag::None;
        //Needs to handle linking once when declaration happens 
        return LinkFlag::Declaration | LinkFlag::Definition;
    }


    RuntimeVariable FunctionBase::BasicExecute(Function* self, ITemplateBody* body, api::vector<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def)
    {
        if (IsValid() == false) {
            report::log("Function '{}' is not valid. Maybe say error later.", IssueType::Apply, def ? IssueLevel::Failure : IssueLevel::Error, GetName());

            //disable this warning.

            return *def;
        }


        Variable* target = nullptr;

        if (args->size() != GetParamCount())
            report::apply::critical("Arg size not compatible with param size ({}/{})", args->size(), parameters.size());



        VisitParameters([&](ParameterInfo& param)
            {

                //Cancelling this for now. It should be used in Invoke, rather than here.
                return;
                int i = param.GetFieldIndex();

                TypeInfo* expected = param.GetType()->FetchTypePolicy(caller);


                if (!expected)
                    report::apply::critical("unexpected?");

                RuntimeVariable check = args[i]->Convert(expected);

                if (check.IsVoid() == true)
                    report::apply::critical("cannot convert argument into parameter {}, {} vs {}", param.GetFieldName(), i, i);

                args[i] = check;
            });



        RuntimeVariable result;

        if (Procedure prod = GetProcedure(); prod)
        {
            ProcedureData data;

            data.runtime = caller;
            data.defOption = def;
            data.function = self;

            auto begin = args->begin();

            std::vector<Variable*> send_args;

            send_args.reserve(GetArgCount());

            if (self->IsMethod() == true)
            {
                target = args[0].Ptr();
                begin++;
            }

            std::transform(begin, args->end(), std::back_inserter(send_args), [&](auto& it) { return it.Ptr(); });

            report _{ IssueType::Runtime };

            prod(result, target, send_args, data);

            if (result.IsRefNegated())
            {
                logger::debug("triggered");
                void* ptr = result.Ptr();

                for (auto& rvar : *args)
                {
                    if (rvar.Ptr() == ptr)
                    {
                        if (rvar.IsReference() == false) {
                            report::fault::critical("Non-reference argument referenced in return.");
                        }

                        result = rvar.AsRef();

                        break;
                    }
                }

            }


            for (auto& rvar : *args) {
                rvar.TryUpdateRef();
            }
        }
        else
        {
            Runtime runtime{ *GetRoutine(), args, caller, body };//The creation of caller yields 2 numbers that should not exist.

            result = runtime.Run();

            //verify

            return result;
        }

        return result;
    }



}