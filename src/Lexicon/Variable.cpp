#include "Lexicon/Variable.h"
#include "Lexicon/RuntimeVariable.h"


namespace LEX
{
    RuntimeVariable Variable::Convert(TypeInfo* to)
    {
        //TODO: Needs cleaning.
        TypeInfo* from = Policy();
        Variable result{};
        if (from)
        {
            from->Convert(*this, result, to);
        }

        return result;
    }
    
    TypeInfo* Variable::CheckVariableType() const
    {
        TypeInfo* result = std::visit([](auto&& lhs) -> TypeInfo* {
            return LEX::GetVariableType(lhs);
            }, _value);

        return result;
    }



}