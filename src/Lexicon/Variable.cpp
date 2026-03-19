#include "Lexicon/Variable.h"
#include "Lexicon/RuntimeVariable.h"


namespace LEX
{
    bool Variable::Convert(TypeInfo* to, Variable& out)
    {
        //TODO: Needs cleaning.
        return to ? to->Convert(*this, out) : false;
    }
    
    TypeInfo* Variable::GetTypeInfo() const
    {
        TypeInfo* result = std::visit([](auto&& lhs) -> TypeInfo* {
            return LEX::GetVariableType(lhs);
            }, value());

        return result;
    }



}