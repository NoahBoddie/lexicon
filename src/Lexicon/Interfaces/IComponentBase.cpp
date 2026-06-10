#include "Lexicon/Interfaces/IComponentBase.h"

#include "Lexicon/Interfaces/IComponent.h"
#include "Lexicon/Engine/Impl/ComponentTraits.h"

namespace LEX
{

    bool IComponentBase::IsType(Type type) const noexcept
    { 
        return GetComponentBase(); 
    }

    const IComponent* IComponentBase::GetComponentBase() const
    {
        return GetComponent()->As<IComponent>();
    }

    const void* IComponentBase::CastComponent(const void* self, ComponentType from, ComponentType to) const
    {
        return ComponentCast(self, from, to);
    }
}