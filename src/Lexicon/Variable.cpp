#include "Lexicon/Variable.h"
#include "Lexicon/RuntimeVariable.h"


namespace LEX
{
    bool Variable::Convert(TypeInfo* to, Variable& out)
    {
        //TODO: Needs cleaning.
        TypeInfo* from = GetTypeInfo();
        return from ? from->Convert(*this, out, to) : false;
    }
    
    TypeInfo* Variable::GetTypeInfo() const
    {
        TypeInfo* result = std::visit([](auto&& lhs) -> TypeInfo* {
            return LEX::GetVariableType(lhs);
            }, value());

        return result;
    }



}