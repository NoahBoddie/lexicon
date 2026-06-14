#pragma once

//*src
#include "Lexicon/Engine/Attribute.h"

namespace LEX
{
    struct Component;
    struct Directory;
    struct SyntaxRecord;
    
    using AttributePtr = std::unique_ptr<Attribute>;



    struct AttributeHandler
    {
        constexpr static size_t uninitialized = -1;

        void LoadAttributes(AttributeOwner* a_this, Directory* parent, SyntaxRecord& record);


        std::span<AttributeBase*> GetAttributes() 
        { 
            if (IsInitialized() && _length) {
                AttributeBase** data = reinterpret_cast<AttributeBase**&>(_data);

                return std::span<AttributeBase*>{data, _length};
            }
            return {}; 
        }

        bool IsInitialized() const noexcept
        {
            return _length != uninitialized;
        }


    private:
        std::unique_ptr<AttributePtr[]> _data = nullptr;
        size_t _length = uninitialized;

    };


}