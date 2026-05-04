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




		virtual void SetDefault(const Variable&) = 0;

		void EmplaceDefault(const Variable& var);


		bool Convert(const Variable& from, Variable& to, TypeInfo* null_override = nullptr) const override final;

	};
}