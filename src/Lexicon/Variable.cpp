#include "Lexicon/Variable.h"
#include "Lexicon/RuntimeVariable.h"


namespace LEX
{
    RuntimeVariable Variable::Convert(Type* to)
    {
        //TODO: Needs cleaning.
        Type* from = Policy();
        Variable result{};
        if (from)
        {
            from->Convert(*this, result, to);
        }

        return result;
    }
    
    Type* Variable::CheckVariableType() const
    {
        Type* result = std::visit([](auto&& lhs) -> Type* {
            return LEX::GetVariableType(lhs);
            }, _value);

        return result;
    }



}