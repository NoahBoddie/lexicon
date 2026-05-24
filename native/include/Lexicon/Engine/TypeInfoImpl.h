#pragma once

#include "Lexicon/TypeInfo.h"


namespace LEX
{
	struct IMPL_A_VERSION(TypeInfo)
	{
	public:
		TypeInfo* GetTypeInfo(ITemplateBody* args) override
		{
			return reinterpret_cast<TypeInfo*>(this);
		}

		bool Convert(const Variable& from, Variable& to, TypeInfo* null_override = nullptr) const override final;

	};
}