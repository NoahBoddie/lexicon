#pragma once

#include "Lexicon/IAttribute.h"

namespace LEX
{
    struct TypeInfo;

    struct AttributeBase : public IAttribute
    {
        TypeInfo* GetType() override
        {
            return _type;
        }
        
        static TypeInfo* GetVariableType(const AttributeBase* attribute)
        {
            if (attribute) {
                return attribute->_type;
            }

            return {};
        }

    INTERNAL:
        TypeInfo* _type = nullptr;
    };

}