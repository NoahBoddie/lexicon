#include "Variable.h"
#include "RuntimeVariable.h"


namespace LEX
{
    RuntimeVariable Variable::Convert(AbstractTypePolicy* to)
    {
        Conversion convert;

        AbstractTypePolicy* from = GetPolicy();

        bool success = from && from->IsConvertibleTo(to, convert, ConversionType::Explicit);
        
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
}