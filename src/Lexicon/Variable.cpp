#include "Lexicon/Variable.h"
#include "Lexicon/RuntimeVariable.h"

#include "Lexicon/TypeInfo.h"

namespace LEX
{
    bool Variable::Convert(TypeInfo* to, Variable& out)
    {
        //TODO: Needs cleaning.
        return to ? to->Convert(*this, out, to) : false;
    }
    
    TypeInfo* Variable::GetTypeInfo() const
    {
        TypeInfo* result = std::visit([](auto&& lhs) -> TypeInfo* {
            return LEX::GetVariableType(lhs);
            }, value());

        return result;
    }


    void DataHelper::DecrementUpdate() const
    {
        if (GetData().HasFlag(VariableFlag::Detached) == true)
        {
            auto var = reinterpret_cast<const Variable*>(this);
            delete var;
            logger::trace("deleted detached variable");
        }

        if (GetData().HasFlag(VariableFlag::Collected) == true) {
            //Send garbage collection.
        }
    }
}