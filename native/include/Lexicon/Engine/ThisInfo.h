#pragma once

#include "Lexicon/Engine/VarIndexInfo.h"
#include "Lexicon/Engine/QualifiedType.h"
#include "Lexicon/Engine/parse_strings.h"


namespace LEX
{
	//Used exclusively to represent "this" rather than parameter
	struct ThisInfo : public IVarIndexInfo
	{
		constexpr ThisInfo() noexcept = default;

		ThisInfo(QualifiedType t) :
			qualifiers{ t },
			type{ t.policy }
		{

		}

		DEFINE_INFO_TYPE(InfoType::ThisInfo)
	public:

		std::string_view GetName() const override
		{
			return parse_strings::this_word;
		}

		ITypeInfo* GetType() const override
		{
			return type;
		}

		//These should have some qualifiers tacked on that are expected of "this"
		Refness GetRefness() override
		{
			return qualifiers.reference;
		}
		Constness GetConstness() override
		{
			return qualifiers.constness;
		}
		QualifierFlag GetQualifierFlags() override
		{
			return qualifiers.flags;
		}

		constexpr size_t GetIndex() const noexcept override { return 0; }


		ITypeInfo* type = nullptr;
		Qualifier qualifiers;
	};

}