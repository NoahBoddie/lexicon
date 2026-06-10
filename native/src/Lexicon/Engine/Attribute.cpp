#include "Lexicon/Engine/Attribute.h"


#include "Lexicon/Interfaces/FormulaManager.h"
#include "Lexicon/IFormula.h"
#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/TypeInfoImpl.h"
#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/parse_strings.h"
#include "Lexicon/FormulaHandler.h"
#include "Lexicon/AnnotatedType.h"
#include "Lexicon/SignatureBase.h"

namespace LEX
{

    std::string_view Attribute::GetName() const
    {
        return _type->GetName();
    }


    bool Attribute::ShouldInnateConstruct(SyntaxRecord& record)
    {
        SyntaxRecord* args = record.FindChild(parse_strings::args);
        //I want to make it so if there are no args it does nothing, but whatever
        if (!args || args->size() != 0) {
            return false;
        }
    
        TypeInfo* type = GetType();

        assert(type);

        return  type->ShouldInnateConstruct();
    }

    bool Attribute::Initialize(Attribute* a_this, SyntaxRecord record, Component* parent, Script* script)
    {
        //validate record?

        _parent = parent;

        //I want to make it so if there are no args it does nothing, but whatever
        if (ShouldInnateConstruct(record) == false) {
            record.GetTag() = parse_strings::constructor;
            record.GetSyntax().type = SyntaxType::AttrCall;//I'd like to give a different syntax that will give the constructor extra permissions.

            ISignature sign;

            SignatureBase* base = &sign;

            base->result = base->target = AnnotatedType{ a_this->GetType() };

            FormulaHandler ctor{};

            //No source for now, I need to make it use a different type that's controllable.
            //std::source_location loc{};


            if (FormulaManager::instance->RequestFormulaFromRecord(sign, {}, a_this->GetName(), record, ctor, script) != 0) {
                //failure
                return false;
            }

            RuntimeVariable result = ctor.formula()->Call(a_this);

            logger::trace("Test attribute result {}", result.Ref() == this);

            //Ensure that result is equal to that.
        }

        //Call something here involving the parent.
        OnTargetValidated();

        return true;
    }

}