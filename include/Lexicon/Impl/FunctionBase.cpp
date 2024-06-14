#include "FunctionBase.h"

#include "Expression.h"
#include "Environment.h"
#include "Declaration.h"
#include "RoutineCompiler.h"
#include "PolicyBase.h"

#include "parse_strings.h"

namespace LEX
{
    using Syntax = Impl::Syntax;
    using SyntaxType = Impl::SyntaxType;




    void FunctionBase::LoadFromRecord(Record& target)
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

    void FunctionBase::OnAttach()
    {
		return;
        Record& target = *GetSyntaxTree();


        Environment* environment = GetEnvironment();

        if (!environment) {
            report::link::error("environ issues cont.");
        }
        
        Record* head_rec = target.FindChild(parse_strings::header);

        if (!head_rec)
            report::compile::fatal("No record named header.");
        //LINK_AFTER

        Declaration header{ *head_rec, environment };

        if (header.Matches(true, Qualifier::Const) == false) {
            report::compile::fatal("Either unexpected qualifiers/specifiers or no type when type expected.");
        }

        //ITypePolicy* policy = environment->TEMPSearchType(target.FindChild("type")->GetFront().GetTag());

        QualifiedType type = header;

        //GENERIC_SPACE

        SetReturnType(type);


        //STATIC_CHECK

        bool method = false;

        if (auto extend = target.FindChild(parse_strings::extends); extend)
        {
            method = true;
            
            auto& tag = extend->GetFront().GetTag();

            _targetType = GetPolicyFromSpecifiers(*extend, environment);
            //_targetType = environment->TEMPSearchType(tag);

            if (!_targetType) {
                report::link::error("No type found with the name '{}' (tag not accurate anymore)", tag);
            }
            else {
                logger::debug("I, {}, have type {}", _name, (uint64_t)_targetType);
            }

            //Qualifiers like const are put here depending on if the function is const. 
            // We don't have those post declarations yet.
            auto& param = parameters.emplace_back(QualifiedType{ type }, parse_strings::this_word, 0);


            //Include things like whether this is
            __thisInfo = std::make_unique<ParameterInfo>(QualifiedType{ type }, parse_strings::this_word, 0);
        }


        for (int64_t i = 0; auto & node : target.FindChild(parse_strings::parameters)->GetChildren())
        {
            Record* node_head = node.FindChild(parse_strings::header);

            if (!node_head)
                report::compile::fatal("No record named header.");

            Declaration header{ *node_head, environment };

            //Unlike the return type, clearly parameters cannot be static, that's a compiling error.
            if (header.Matches(true, Qualifier::Const | Qualifier::Runtime, DeclareSpecifier::Const) == false) {
                report::compile::fatal("Either unexpected qualifiers/specifiers or no type when type expected.");
            }

            QualifiedType type = header;

            //auto& tag = node.FindChild("type")->GetFront().GetTag();

            //ITypePolicy* policy = environment->TEMPSearchType(node.FindChild("type")->GetFront().GetTag());

            //if (!policy) {
            //    report::link::error("Parameter type '{}' couldn't be found", tag);
            //}

            //GENERIC_SPACE
            
            //auto& param = parameters.emplace_back(type, node.GetTag(), method + i++);
            auto& param = parameters.emplace_back(type, node.GetTag(), static_cast<uint32_t>(parameters.size()));

            assert(param.GetType());
        }
       
        _routine = RoutineCompiler::Compile(target, this, GetEnvironment());
    }

    void FunctionBase::SetReturnType(QualifiedType type)
    {
        _returnType = type;
    }



    LinkResult FunctionBase::OnLink(LinkFlag flags)
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
                report::compile::fatal("No record named header.");
            //LINK_AFTER

            Declaration header{ *head_rec, environment };

            if (header.Matches(true, Qualifier::Const) == false) {
                report::compile::fatal("Either unexpected qualifiers/specifiers or no type when type expected.");
            }

            //ITypePolicy* policy = environment->TEMPSearchType(target.FindChild("type")->GetFront().GetTag());

            QualifiedType type = header;

            //GENERIC_SPACE

            SetReturnType(type);


            //STATIC_CHECK

            bool method = false;

            if (auto extend = target.FindChild(parse_strings::extends); extend)
            {
                method = true;

                auto& tag = extend->GetFront().GetTag();

                _targetType = GetPolicyFromSpecifiers(*extend, environment);
                //_targetType = environment->TEMPSearchType(tag);

                if (!_targetType) {
                    report::link::error("No type found with the name '{}' (tag not accurate anymore)", tag);
                }
                else {
                    logger::debug("I, {}, have type {}", _name, (uint64_t)_targetType);
                }

                //Qualifiers like const are put here depending on if the function is const. 
                // We don't have those post declarations yet.
				auto& param = parameters.emplace_back(QualifiedType{ type }, parse_strings::this_word, 0);


                //Include things like whether this is
                __thisInfo = std::make_unique<ParameterInfo>(QualifiedType{ type }, parse_strings::this_word, 0);
            }


            for (int64_t i = 0; auto & node : target.FindChild(parse_strings::parameters)->GetChildren())
            {
                Record* node_head = node.FindChild(parse_strings::header);

                if (!node_head)
                    report::compile::fatal("No record named header.");

                Declaration header{ *node_head, environment };

                //Unlike the return type, clearly parameters cannot be static, that's a compiling error.
                if (header.Matches(true, Qualifier::Const | Qualifier::Runtime, DeclareSpecifier::Const) == false) {
                    report::compile::fatal("Either unexpected qualifiers/specifiers or no type when type expected.");
                }

                QualifiedType type = header;

                //auto& tag = node.FindChild("type")->GetFront().GetTag();

                //ITypePolicy* policy = environment->TEMPSearchType(node.FindChild("type")->GetFront().GetTag());

                //if (!policy) {
                //    report::link::error("Parameter type '{}' couldn't be found", tag);
                //}

                //GENERIC_SPACE

                //auto& param = parameters.emplace_back(type, node.GetTag(), method + i++);
                auto& param = parameters.emplace_back(type, node.GetTag(), static_cast<uint32_t>(parameters.size()));

                assert(param.GetType());
            }

            break;
        }

        case LinkFlag::Definition:
        {
            _routine = RoutineCompiler::Compile(target, this, GetEnvironment());
            break;
        }

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


}