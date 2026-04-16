#pragma once

namespace LEX
{

    ENUM(DirectiveType, uint8_t)
    {
        Requirement,
        Conditional,
        Prefunc,
        Format,
        Total,
        Header,
        None,
    };

    struct Directive
    {
        DirectiveType type{ DirectiveType::Total };
        Column column{ 0 };
        Line line{ 0 };


        std::string Print() const
        {
            if (type == DirectiveType::Total)
                return "Syntax: Header";
            else
                return std::format("Syntax: {} (col: {}/ line: {})", magic_enum::enum_name(type), column, line);
        }

        std::string location() const
        {
            return std::format("(line: {}, col: {})", line, column);
        }

        operator std::string() { return Print(); }


        Directive() = default;
        Directive(DirectiveType t, Column c = 0, Line l = 0) :
            type{ t },
            column{ c },
            line{ l }
        {
        }
    };
    static_assert(sizeof(Directive) == 0x8);

}