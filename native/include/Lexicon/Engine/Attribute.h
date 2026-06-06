#pragma once

#include "Lexicon/AttributeBase.h"
#include "Lexicon/Engine/Component.h"

//*src
#include "Lexicon/Interfaces/Info.h"

namespace LEX
{
    //TODO: Instead of making this a component I'll use link complete in order to handle attribute addition
    // Attributes themselves will still have a function they use to load themselves (namely for the purpose)
    // of calling upon their constructors.
    struct Attribute : public AttributeBase
    {
        //Base object of both custom and native attribute
        IComponent* GetCompParent() override final
        {
            return _parent ? _parent->GetComponentBase() : nullptr;;
        }

        Info* GetInfoParent() override final
        {
            return dynamic_cast<Info*>(_parent);
        }

        std::string_view GetName() const;

        virtual void LoadAttribute(SyntaxRecord& record)
        {

        }

        //I'm thinking these can only go on components.
        Component* _parent = nullptr;
    };

}