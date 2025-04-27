#pragma once

#include "Lexicon/TypeInfo.h"
#include "Lexicon/Variable.h"

namespace LEX
{
	bool ITypeInfo::Convert(const Variable& from, Variable& to, const ITypeInfo* to_type) const
	{
		Conversion convert;

		TypeInfo* from_type = from.Policy();

		//I want to make a variable vtable to handle this at a later point
		bool success = from_type && from_type->IsConvertibleTo(to_type, to_type, convert, ConversionFlag::Explicit);

		if (success)
		{
			to = from;

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

		return success;
	}
}