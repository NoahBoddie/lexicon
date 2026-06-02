#pragma once

#include "Lexicon/TypeInfo.h"

#include "Lexicon/AttributeData.h"

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




		virtual bool RegisterAttributeData(AttributeBuilder builder)
		{
			return false;
		}

		template <std::derived_from<AttributeData> T>
		bool RegisterAttribute()
		{
			return RegisterAttributeData(AttributeData::Create<T>);
		}
	};
}