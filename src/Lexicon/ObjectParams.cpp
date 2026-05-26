#include "Lexicon/ObjectParams.h"

#include "Lexicon/Object.h"

namespace LEX
{
    ObjectParams::ObjectParams(const Object& obj) :
        data{ obj.data(true) },
        type{ obj.type },
        context{ obj.GetContext() }
    {

    }
}