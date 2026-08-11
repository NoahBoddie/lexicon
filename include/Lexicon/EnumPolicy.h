#pragma once

#include "Lexicon/EnumEntry.h"

namespace LEX
{
    struct TypeInfo;


    ENUM(EnumFlag)
    {
        None = 0,
        Scoped = 1 << 0,
    };

    namespace Version
    {
        namespace _1
        {
            struct M_INTERFACE_VERSION(EnumPolicy)
            {
                virtual TypeInfo* GetType() const = 0;
                virtual EnumFlag GetEnumFlags() const = 0;
                virtual std::optional<EnumEntry> GetEntryFromValue(intptr_t val) const = 0;
                virtual std::optional<EnumEntry> GetEntryFromName(const std::string_view& name) const = 0;
            };
        }

        CURRENT_VERSION(EnumPolicy, 1);
    }


    struct IMPL_VERSION(EnumPolicy)
    {
    public:
        bool IsScoped() const;
        intptr_t GetDefault() const;
        std::string GetEnumName(intptr_t value) const;


    };

}