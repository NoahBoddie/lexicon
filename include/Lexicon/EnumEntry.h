#pragma once

namespace LEX
{
    //Give this its own file
    ENUM(EntryType, uint8_t)
    {
        Flag,
        Value,
        Hardcoded,
    };

    struct EnumEntry
    {
        EntryType type;
        std::string_view name;
        intptr_t value;
    };
}