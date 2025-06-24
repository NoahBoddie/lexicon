#include "Lexicon/TypeInfo.h"
#include "Lexicon/Variable.h"
#include "Lexicon/TypeID.h"

#include "Lexicon/RuntimeVariable.h"

namespace LEX
{
	void TypeInfo::EmplaceDefault(Variable& var)
	{
		//The void type would have 0 as it's value.
		if (GetTypeID() != 0 && GetDefault().IsVoid() == true) {
			return SetDefault(var);
		}
	}


}