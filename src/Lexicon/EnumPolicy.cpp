#include "Lexicon/EnumPolicy.h"

#include "Lexicon/TypeInfo.h"

namespace LEX
{

    bool EnumPolicy::IsScoped() const
    {
        return GetEnumFlags() & EnumFlag::Scoped;
    }

    intptr_t EnumPolicy::GetDefault() const
    {
        std::optional<EnumEntry> result = GetEntryFromName("Default");

        if (result) {
            return result.value().value;
        }

        return 0;
    }
 
    std::string EnumPolicy::GetEnumName(intptr_t value) const
    {
        //I think the strings will work like so,
        //If scoped, the name of the type goes first, followed by "::"
        //If contains flags, all entries will be nested in []
        //The main value will be front.
        //If no main value is found, the remainder becomes an unknown value

        std::string result;
        std::string scoped_string;
        std::optional<EnumEntry> entry;

        bool valued = false;
        bool flagged = false;


        //I want to reorganize the way this is done. Possibly make it look like ScopeName::[flags|value]

        auto get_string = [&](std::string_view view) -> std::string
            {
                return std::format("{}{}{}", result.empty() ? "|" : "", scoped_string, view);
            };


        if (IsScoped() == true) {
            scoped_string = std::format("{}::", GetType()->GetName());
        }


        for (std::optional<EnumEntry> it = GetEntryFromValue(value); it.has_value(); it = GetEntryFromValue(value))
        {
            auto& entry = it.value();

            result += get_string(entry.name);

            switch (entry.type)
            {
            case EntryType::Flag:
                flagged = true;
                break;
            case EntryType::Value:
                valued = true;
                break;
            }


            if (!entry.value)
                break;

            value &= ~entry.value;

            if (!value)
                break;

        }

        if (value)
            result += get_string(std::format("Unk({})", value));

        if (flagged)
        {
            result = std::format("[{}]", result);
        }

        if (result.empty() == true) {
            result = "<none>";
        }

        return result;
    }

}