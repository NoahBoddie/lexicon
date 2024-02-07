#include "FunctionBase.h"

#include "Expression.h"
#include "Environment.h"
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
            RGL_LOG(critical, "Not a function, cannot load.");
            throw nullptr;
        }

        _name = target.GetTag();
	}

    void FunctionBase::OnAttach()
    {
        Record& target = *GetSyntaxTree();


        Environment* environment = GetEnvironment();

        if (!environment) {
            logger::critical("environ issues cont.");
            throw nullptr;
        }
        //LINK_AFTER

        ITypePolicy* policy = environment->TEMPSearchType(target.FindChild("type")->GetFront().GetTag());

        //GENERIC_SPACE

        SetReturnType(policy);

        for (size_t i = 0; auto & node : target.FindChild("params")->GetChildren())
        {
            auto tag = node.FindChild("type")->GetFront().GetTag();

            ITypePolicy* policy = environment->TEMPSearchType(node.FindChild("type")->GetFront().GetTag());

            if (!policy) {
                RGL_LOG(critical, "Parameter type '{}' couldn't be found", tag);
                throw nullptr;
            }

            //GENERIC_SPACE

            logger::debug("test {}", !parameters.emplace_back(policy, i++, node.GetTag()).GetTypePolicy());
        }

        _routine = RoutineCompiler::Compile(target, this, GetEnvironment());
    }

    void FunctionBase::SetReturnType(ITypePolicy* policy)
    {
        temp_returnType = policy;
    }
}