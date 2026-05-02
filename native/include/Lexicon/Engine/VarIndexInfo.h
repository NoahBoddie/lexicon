#pragma once
#include "Lexicon/Qualifier.h"
#include "Lexicon/Interfaces/VarInfo.h"
#include "Lexicon/Engine/QualifiedType.h"
namespace LEX
{
	struct IVarIndexInfo : public VarInfo
	{
		virtual size_t GetIndex() const noexcept = 0;
	};


    struct VarIndexInfo : public IVarIndexInfo
    {
		constexpr VarIndexInfo() noexcept = default;

		VarIndexInfo(QualifiedType t, uint32_t i) :
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

		size_t GetIndex() const noexcept override
		{
			return index;
		}
	
        std::string _name;
        ITypeInfo* type = nullptr;
        Qualifier qualifiers;
	protected:
        uint32_t index;
    };
}