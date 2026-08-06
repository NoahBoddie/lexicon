#pragma once

#include "Lexicon/Engine/ConcreteType.h"

namespace LEX
{
	struct IntrinsicType : public ConcreteType
	{
	public:
		IntrinsicType(uint32_t i, const Variable& def) :
			_default{ def },
			ConcreteType{ i } 
		{}

		IntrinsicType(std::string_view name, TypeOffset offset, const Variable& def) : 
			_default {def},
			ConcreteType{ name, offset } 
		{}



		Variable GetDefault() const override
		{
			if (auto policy = GetObjectPolicy(); !policy)
				return _default;

			return ConcreteType::GetDefault();
		}

		Variable GetVariable() const override
		{
			if (auto policy = GetObjectPolicy(); !policy)
				return _default;
			
			return ConcreteType::GetDefault();

		}

	private:
		//TODO: Make this only for intrinsic types, nothing else needs it
		Variable _default{};


	};
}