#pragma once


#include "MemberInfo.h"

namespace LEX
{

	struct VariableInfo : public MemberInfo
	{
		VariableInfo() = default;

		//no qualifiers for now.
		VariableInfo(ITypePolicy* t, size_t i) : _index{ i }, _type{ t }
		{

		}


		FieldType GetFieldType() const override
		{
			return FieldType::Param;
		}


		ITypePolicy* _type = nullptr;
		size_t _index = max_value<size_t>;
	};

}
