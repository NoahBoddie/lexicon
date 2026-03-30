#pragma once

#include "Lexicon/TypeInfo.h"


namespace LEX
{
	struct TypeInfo : public TypeInfoAbstract
	{
		virtual void SetDefault(const Variable&) = 0;

		void EmplaceDefault(const Variable& var);


		bool Convert(const Variable& from, Variable& to, TypeInfo* null_override = nullptr) const override final;

	};
}