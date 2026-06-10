#include "Lexicon/Interfaces/AttributeOwner.h"

#include "Lexicon/Interfaces/Info.h"
#include "Lexicon/Interfaces/IComponent.h"
namespace LEX
{
    void* AttributeOwner::AsType(Type type)
    {
        switch (type)
        {
        case kInfo:
            return dynamic_cast<Info*>(this);
        case kIComponent:
            return dynamic_cast<IComponent*>(this);
        default:
            return nullptr;
        }
    }

    bool AttributeOwner::IsType(Type type) const noexcept
    {
        return false;
    }
}