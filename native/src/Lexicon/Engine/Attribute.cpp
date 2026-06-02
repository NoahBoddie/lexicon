#include "Lexicon/Engine/Attribute.h"

#include "Lexicon/Engine/TypeInfoImpl.h"

namespace LEX
{

    std::string_view Attribute::GetName() const
    {
        return _type->GetName();
    }
}