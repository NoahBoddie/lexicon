#pragma once

#include "Lexicon/AttributeBase.h"
#include "Lexicon/Engine/Component.h"

//*src
#include "Lexicon/Interfaces/Info.h"

namespace LEX
{
    struct Script;
    struct SyntaxRecord;

    //TODO: Instead of making this a component I'll use link complete in order to handle attribute addition
    // Attributes themselves will still have a function they use to load themselves (namely for the purpose)
    // of calling upon their constructors.
    struct Attribute : public AttributeBase
    {
        AttributeOwner* GetParent() override
        {
            return _parent;
        }

        std::string_view GetName() const override;

        bool ShouldInnateConstruct(SyntaxRecord& record);

        bool Initialize(SyntaxRecord record, AttributeOwner* parent, Script* script);

        //I'm thinking these can only go on components.
        AttributeOwner* _parent = nullptr;
    };

}