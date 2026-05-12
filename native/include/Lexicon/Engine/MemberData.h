#pragma once

#include "Lexicon/Engine/FieldInfo.h"
#include "Lexicon/MemberPointer.h"

namespace LEX
{

    struct IFunction;

    //This is used only for non-static fields and virtual methods
    struct MemberData
    {
        std::vector<FieldInfo> fields;
        //I want to use I function for this, but it will need to be smoothed over with places that want a base
        std::map<MemberPointer, IFunction*> methods;
    };
}