#include "AbstractTypePolicy.h"
#include "Variable.h"
#include "TypeID.h"

#include "RuntimeVariable.h"

namespace LEX
{
	void AbstractTypePolicy::EmplaceDefault(Variable& var)
	{
		//The void type would have 0 as it's value.
		if (GetTypeID() != 0 && GetDefault().IsVoid() == true) {
			return SetDefault(var);
		}
	}


}