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
        void LoadAttributes(Component* a_this, Directory* parent, SyntaxRecord& record);


    private:
        std::unique_ptr<AttributePtr[]> _data = nullptr;
        size_t _length = 0;

    };


}