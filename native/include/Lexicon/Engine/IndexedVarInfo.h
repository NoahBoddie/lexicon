#pragma once
#include "Lexicon/Qualifier.h"
#include "Lexicon/Interfaces/VarInfo.h"
#include "Lexicon/Engine/QualifiedType.h"
namespace LEX
{

    struct IndexedVarInfo : public VarInfo
    {
		constexpr IndexedVarInfo() noexcept = default;

		IndexedVarInfo(QualifiedType t, uint32_t i) :
			qualifiers{ t },
			type{ t.policy },
			index{ i }
		{

		}

        std::string_view GetName() const override
        {
            return _name;
        }

		ITypeInfo* GetType() const override
		{
			return type;
		}

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
	
        std::string _name;
        ITypeInfo* type = nullptr;
        Qualifier qualifiers;
        uint32_t index;
    };
}