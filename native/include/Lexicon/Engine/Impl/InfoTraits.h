#pragma once

#include "Lexicon/InfoType.h"

#include "Lexicon/Interfaces/Info.h"
#include "Lexicon/Interfaces/VarInfo.h"
#include "Lexicon/Engine/LocalInfo.h"
#include "Lexicon/Engine/ParameterInfo.h"
#include "Lexicon/Engine/GlobalBase.h"

namespace LEX
{
    struct InfoTraits
    {
#define LHS_INFO_TRAITS(mc_type) \
    case mc_type::INFO_TYPE:{\
        return LeftInfoCast<mc_type>(self, to);\
    }

#define RHS_INFO_TRAITS(mc_type) \
    case mc_type::INFO_TYPE:{\
        return RightInfoCast<T, mc_type>(self);\
    }
        //TODO: Make a warning when the true type can convert to the target type, but the access type can't convert to the true type.


#define INFO_TRAITS(mc_type) \
    case mc_type::INFO_TYPE:\
        if constexpr ((std::is_base_of_v<mc_type, From> || std::is_base_of_v<From, mc_type>) &&\
            std::is_base_of_v<To, mc_type>){\
            return static_cast<const mc_type*>(self);\
        }\
        break;

#define INFO_NAMES Info, VarInfo, LocalInfo, ParameterInfo, ThisInfo, GlobalBase



        template <typename From, typename To>
        static const To* RightInfoCast(const From* self)
        {
            InfoType type;

            if constexpr (requires(const From* it) { { it->GetInfoType() } ->std::convertible_to<InfoType>; })
            {
                type = self->GetInfoType();
            }
            else
            {
                //This roughly should never actually happe I think, and if it's reached it should scream.
                type = From::INFO_TYPE;
            }

            switch (type)
            {
                MAP(INFO_TRAITS, INFO_NAMES);
            default:
                report::fault::critical("Unknown InfoType {} detected", magic_enum::enum_name(type));
                break;
            }

            return nullptr;
        }

        template<typename T>
        static const void* LeftInfoCast(const void* ptr, InfoType to)
        {
            const T* self = reinterpret_cast<const T*>(ptr);

            switch (to)
            {
                MAP(RHS_INFO_TRAITS, INFO_NAMES);

            default:
                report::fault::critical("Unknown InfoType {} detected", magic_enum::enum_name(to));
                break;
            }

            return nullptr;
        }

        //From in this case is the pointer it's being percieved as, to is the goal
        static const void* InfoCast(const void* self, InfoType from, InfoType to)
        {
            if (self)
            {
                switch (from)
                {
                    MAP(LHS_INFO_TRAITS, INFO_NAMES);

                default:
                    report::fault::critical("Unknown InfoType {} detected", magic_enum::enum_name(from));
                    break;
                }
            }
            return nullptr;
        }
    };

#undef LHS_INFO_TRAITS
#undef RHS_INFO_TRAITS
#undef INFO_TRAITS
#undef INFO_NAMES

}