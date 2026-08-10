#pragma once

#include "Lexicon/Interfaces/VarInfo.h"

namespace LEX
{

    struct EnumInfo : public VarInfo
    {
        std::string_view GetName() const
        {
            return name.view();
        }

        virtual ITypeInfo* GetType() const
        {
            return constant.GetTypeInfo();
        }

        virtual Refness GetRefness() override
        {
            return Refness::Temp;
        }
        virtual Constness GetConstness()
        {
            return Constness::Const;
        }

        virtual QualifierFlag GetQualifierFlags() override
        {
            return QualifierFlag::None;
        }

        void SetConstant(EnumPolicy* policy)
        {
            
            constant = Enumeration{ policy, value };
        }

        //TODO: All this needs work for sorting.
        bool operator <(const EnumInfo& other) const noexcept
        {
            if (type != other.type) {
                return type < other.type;
            }

            switch (type)
            {
                //Hard coded stuff goes lowest.
            case EntryType::Hardcoded:
                return false;

            case EntryType::Flag:
                return std::popcount(uvalue) < std::popcount(other.uvalue);

            default:
                return uvalue < other.uvalue;

            }

        }

        EnumEntry GetEntry() const
        {
            EnumEntry result{};
            result.name = name.view();
            result.value = value;
            result.type = type;
            return result;
        }

        String name;
        EntryType type{};
        union
        {
            intptr_t value{};
            uintptr_t uvalue;
        };

        Variable constant{};
    };


}