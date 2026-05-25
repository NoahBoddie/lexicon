#pragma once

#include "Lexicon/TypeID.h"
#include "Lexicon/ObjectStorage.h"

namespace LEX
{
    struct ObjectInfoData;

    namespace detail
    {
        template <typename T>
        concept has_info_data = requires() {
            { T::OBJECT_INFO_DATA } -> std::same_as<const ObjectInfoData&>;
            { std::bool_constant<(T::OBJECT_INFO_DATA, true)>() } -> std::same_as<std::true_type>;
        };
    }



    struct ObjectInfoData
    {
        consteval ObjectInfoData(uintptr_t ver, uintptr_t inc, const std::string_view& cat) noexcept :
            VERSION{ ver }, INCOMPATIBLE{ inc }, CATEGORY{ cat } {
        }
        consteval ObjectInfoData(uintptr_t ver, uintptr_t inc, const std::string_view& cat, TypeOffset range) noexcept :
            VERSION{ ver }, INCOMPATIBLE{ inc }, CATEGORY{ cat }, RANGE{ range } {
        }
        consteval ObjectInfoData(uintptr_t ver, uintptr_t inc, const std::string_view& cat, ObjectStorage stor) noexcept :
            VERSION{ ver }, INCOMPATIBLE{ inc }, CATEGORY{ cat }, STORAGE{ stor } {
        }
        consteval ObjectInfoData(uintptr_t ver, uintptr_t inc, const std::string_view& cat, TypeOffset range, ObjectStorage stor) noexcept :
            VERSION{ ver }, INCOMPATIBLE{ inc }, CATEGORY{ cat }, RANGE{ range }, STORAGE{ stor } {
        }


        uintptr_t VERSION{};
        ObjectStorage STORAGE{};
        std::string_view CATEGORY{};
        TypeOffset RANGE{};
        uintptr_t INCOMPATIBLE{}; //Imcompatible below this version.

    };

    namespace detail
    {
        //Type only really exists so I don't have to put version first, it looks ugly
        template <uintptr_t Version = 1, uintptr_t Incompatible = Version>
        struct VersionedObjectInfoData : public ObjectInfoData
        {

            consteval VersionedObjectInfoData(const std::string_view& cat) noexcept :
                ObjectInfoData{ Version, Incompatible, cat } {
            }
            consteval VersionedObjectInfoData(const std::string_view& cat, TypeOffset range) noexcept :
                ObjectInfoData{ Version, Incompatible, cat, range } {
            }
            consteval VersionedObjectInfoData(const std::string_view& cat, ObjectStorage stor) noexcept :
                ObjectInfoData{ Version, Incompatible, cat, stor } {
            }
            consteval VersionedObjectInfoData(const std::string_view& cat, TypeOffset range, ObjectStorage stor) noexcept :
                ObjectInfoData{ Version, Incompatible, cat, range, stor } {
            }
        };
    }

#define OBJECT_INFO_DATA(mc_ver) static constexpr ::LEX::ObjectInfoData OBJECT_INFO_DATA = \
        ::LEX::detail::VersionedObjectInfoData<mc_ver>
}