#pragma once

#include "Lexicon/RelateType.h"

namespace LEX
{
    struct Directory;

    struct Relationship
    {
        RelateType type = RelateType::None;
        //Free space here.
        Directory* to = nullptr;
        //mutable std::atomic<int> _refs = 1;

        operator Directory* ()
        {
            return to;
        }


        constexpr auto operator<=>(const Relationship& other) const noexcept
        {
            if (auto res = type <=> other.type; res != std::strong_ordering::equivalent) {
                return res;
            }

            return to <=> other.to;
        }
    };

}