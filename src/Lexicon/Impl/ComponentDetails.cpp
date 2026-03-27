#include "Lexicon/Impl/ComponentDetails.h"

#include "Lexicon/Engine/Impl/ComponentTraits.h"

#include "Lexicon/Interfaces/IComponentBase.h"
//This is where I'll move the functions that aren't interface in the other 2.

#include "Lexicon/ITypeInfo.h"

namespace LEX
{
    TypeInfo* IComponentBase::GetTypeFromOffset(uint16_t offset)
    {
        ITypeInfo* type = IdentityManager::instance->GetTypeByOffset("REFLECT", offset);
        return type->FetchTypePolicy(nullptr);
    }

    const void* IComponentBase::Cast(const void* self, ComponentType from, ComponentType to) const
    {
        return ComponentCast(self, from, to);
    }
}