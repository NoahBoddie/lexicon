#include "Lexicon/Engine/TypeInfoImpl.h"


#include "Lexicon/Variable.h"

#include "Lexicon/Engine/Convert.h"

namespace LEX
{


	bool TypeInfo::Convert(const Variable& from, Variable& to, TypeInfo* null_override) const
	{
		Conversion convert;

		TypeInfo* from_type = from.GetTypeInfo();

		//I want to make a variable vtable to handle this at a later point
		bool success = from_type;

		if (!from_type)
			return false;

		if (null_override && from_type->IsReferenceType() && from.IsValueZero() == true) {
			from_type = null_override;
		}

		auto result = from_type->IsConvertibleTo(this, this, convert, ConversionFlag::Explicit);


		if (!result)
			return false;

		to = convert.Run(from, result);


		{

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

		return true;
	}


}