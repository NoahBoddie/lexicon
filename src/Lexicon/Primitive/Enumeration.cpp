#include "Lexicon/Primitive/Enumeration.h"

#include "Lexicon/EnumPolicy.h"

namespace LEX
{
    Enumeration::Enumeration(EnumPolicy* policy) : _policy{ policy }, _values{ policy->GetDefault() }{}


    TypeInfo* Enumeration::GetVariableType(const Enumeration* it)
    {
        if (it) {
            it->_policy->GetType();
        }

        //TODO: No type to designate a generic enum yet
        return nullptr;
    }


    Enumeration::operator Number() const
    {
        return _values;
    }
}