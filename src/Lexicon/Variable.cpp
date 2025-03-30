#include "Lexicon/Variable.h"
#include "Lexicon/RuntimeVariable.h"


namespace LEX
{
    RuntimeVariable Variable::Convert(Type* to)
    {
        Conversion convert;

        Type* from = Policy();

        bool success = from && from->IsConvertibleTo(to, to, convert, ConversionFlag::Explicit) > convertFailure;
        
        if (success)
        {
            return *this;

            //TODO:I want IFunction to have a convert function here, where I can call upon convert for this.
            /*
            if (convert) {
                return convert(*this);
            }
            else {
                return *this;
            }
            //*/
        }

        return {};
    }
    
    Type* Variable::CheckVariableType() const
    {
        Type* result = std::visit([](auto&& lhs) -> Type* {
            return LEX::GetVariableType(lhs);
            }, _value);

        return result;
    }



}