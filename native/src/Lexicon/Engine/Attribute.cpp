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

    bool Attribute::Initialize(SyntaxRecord record, AttributeOwner* parent, Script* script)
    {
        //validate record?

        if (CanAllowAttach(parent) == false) {
            report::compile::failure("Restrictions of attribute '{}' have not been met.",
                GetName());

            return false;
        }

        for (auto& attribute : _type->GetAttributes()) {
            if (attribute->CanAttachedAllow(parent, this) == false) {
                //Message that the restrictions of this type have not been met
                report::compile::failure("Restrictions of attribute '{}' have not been met.",
                    attribute->GetName());
                return false;
            }
        }

        //At some later point, I'd love for something that's for between already added attributes to possibly exclude one another.



        _parent = parent;

        //I want to make it so if there are no args it does nothing, but whatever
        if (ShouldInnateConstruct(record) == false) {
            record.GetTag() = parse_strings::constructor;
            record.GetSyntax().type = SyntaxType::AttrCall;//I'd like to give a different syntax that will give the constructor extra permissions.

            ISignature sign;

            SignatureBase* base = &sign;

            base->result = base->target = AnnotatedType{ GetType() };

            FormulaHandler ctor{};

            //No source for now, I need to make it use a different type that's controllable.
            //std::source_location loc{};


            if (FormulaManager::instance->RequestFormulaFromRecord(sign, {}, GetName(), record, ctor, script) != 0) {
                //failure
                return false;
            }

            RuntimeVariable result = ctor.formula()->Call(this);

            logger::trace("Test attribute result {}", result.Ref() == this);

            //Ensure that result is equal to that.
        }

        //Call something here involving the parent.
        try
        {
            OnTargetValidated();
        }
        catch (CompileError& error)
        {
            return false;
        }


        return true;
    }

}