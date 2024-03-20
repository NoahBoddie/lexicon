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
		
		size_t GetFieldIndex() const override
		{
			return _index;
		}

		FieldType GetFieldType() const override { return FieldType::Variable; }

		ITypePolicy* GetTypePolicy() const override
		{
			return _type;
		}


		ITypePolicy* _type = nullptr;
		size_t _index = max_value<size_t>;
	};

}
