#pragma once

#include "Lexicon/Interfaces/IComponentBase.h"

namespace LEX
{
    struct IComponent : public IComponentBase
    {
        DEFINE_COMPONENT_OFFSET(ComponentType::IComponent)
    };
}