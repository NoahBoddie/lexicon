#include "Variable.h"
#include "RuntimeVariable.h"


namespace LEX
{
    RuntimeVariable Variable::Convert(AbstractTypePolicy* to)
    {
        Conversion convert;

        AbstractTypePolicy* from = Policy();

        bool success = from && from->IsConvertibleTo(to, to, convert, ConversionType::Explicit) > convertFailure;
        
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