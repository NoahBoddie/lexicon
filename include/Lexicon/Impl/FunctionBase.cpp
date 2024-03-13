#include "FunctionBase.h"

#include "Expression.h"
#include "Environment.h"
#include "DeclareHeader.h"
#include "RoutineCompiler.h"
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
        Record& target = *GetSyntaxTree();


        Environment* environment = GetEnvironment();

        if (!environment) {
            report::link::error("environ issues cont.");
        }
        
        Record* head_rec = target.FindChild("<header>");

        if (!head_rec)
            report::compile::fatal("No record named header.");
        //LINK_AFTER

        DeclareHeader header{ *head_rec, environment };

        if (header.Matches(true, BasicQualifier::Const) == false) {
            report::compile::fatal("Either unexpected qualifiers/specifiers or no type when type expected.");
        }

        //ITypePolicy* policy = environment->TEMPSearchType(target.FindChild("type")->GetFront().GetTag());

        ITypePolicy* policy = header.policy;

        //GENERIC_SPACE

        SetReturnType(policy);


        //STATIC_CHECK

        bool method = false;

        if (auto extend = target.FindChild("extend"); extend)
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
        }


        for (size_t i = 0; auto & node : target.FindChild("params")->GetChildren())
        {
            Record* node_head = node.FindChild("<header>");

            if (!node_head)
                report::compile::fatal("No record named header.");

            DeclareHeader header{ *node_head, environment };

            //Unlike the return type, clearly parameters cannot be static, that's a compiling error.
            if (header.Matches(true, BasicQualifier::Const, RuntimeQualifier::All, DeclareSpecifier::Const) == false) {
                report::compile::fatal("Either unexpected qualifiers/specifiers or no type when type expected.");
            }

            ITypePolicy* policy = header.policy;

            //auto& tag = node.FindChild("type")->GetFront().GetTag();

            //ITypePolicy* policy = environment->TEMPSearchType(node.FindChild("type")->GetFront().GetTag());

            //if (!policy) {
            //    report::link::error("Parameter type '{}' couldn't be found", tag);
            //}

            //GENERIC_SPACE

            auto& param = parameters.emplace_back(policy, method + i++, node.GetTag());

            assert(param.GetTypePolicy());
        }

        _routine = RoutineCompiler::Compile(target, this, GetEnvironment());
    }

    void FunctionBase::SetReturnType(ITypePolicy* policy)
    {
        _returnType = policy;
    }
}