#pragma once

#include "Lexicon/Interfaces/IComponentBase.h"

namespace LEX
{
    struct IComponent : public IComponentBase
    {
    public:
        DEFINE_COMPONENT_OFFSET(ComponentType::IComponent, ComponentType::Component)
    private:
    };
}