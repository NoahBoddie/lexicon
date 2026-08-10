#pragma once

//*src
#include "Lexicon/Number.h"
namespace LEX
{
    struct Number;
    struct TypeInfo;
    struct EnumPolicy;

    struct Enumeration
    {
        //I'd like to have a version that gets defaults.

        constexpr Enumeration(EnumPolicy* policy, intptr_t value) noexcept : _policy{ policy }, _values{ value }
        {
        }

        Enumeration(EnumPolicy* policy);


        static TypeInfo* GetVariableType(const Enumeration* it);


        constexpr std::strong_ordering operator <=> (const Enumeration& other) const = default;

        //constexpr std::strong_ordering operator <=> (Number other) const{}
        //constexpr bool operator==(const Number& other) const{return operator<=>(other) == std::strong_ordering::equal;}

        operator Number() const;

    INTERNAL:
        intptr_t    _values{};
        EnumPolicy* _policy{};




    };
    REQUIRED_SIZE(Enumeration, 0x10);

}