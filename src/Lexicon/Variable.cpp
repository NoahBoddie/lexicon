#include "Lexicon/Variable.h"
#include "Lexicon/RuntimeVariable.h"


namespace LEX
{
    RuntimeVariable Variable::Convert(TypeInfo* to)
    {
        //TODO: Needs cleaning.
        TypeInfo* from = GetTypeInfo();
        Variable result{};
        if (from)
        {
            from->Convert(*this, result, to);
        }

        return result;
    }
    
    TypeInfo* Variable::GetTypeInfo() const
    {
        TypeInfo* result = std::visit([](auto&& lhs) -> TypeInfo* {
            return LEX::GetVariableType(lhs);
            }, value());

        return result;
    }



}