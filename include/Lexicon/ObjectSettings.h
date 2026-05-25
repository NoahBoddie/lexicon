#pragma once

#include "Lexicon/ObjectInfoData.h"

namespace LEX
{
    //This will be what object info is now, where the settings exist externally
    template<typename T>
    struct ObjectSettings;



    template <typename T>
    concept has_object_info = is_complete_type<ObjectSettings<std::remove_cvref_t<T>>>::value &&
        detail::has_info_data<ObjectSettings<std::remove_cvref_t<T>>>;


    template<detail::has_info_data T>
    struct ObjectSettings<T>
    {
        constexpr static auto OBJECT_INFO_DATA = std::remove_cvref_t<T>::OBJECT_INFO_DATA;
    };

    //put safeties on all of these
    template <typename T>
    consteval uintptr_t GetObjectVersion()
    {
        return ObjectSettings<T>::OBJECT_INFO_DATA.VERSION;
    }

    template <typename T>
    consteval uintptr_t GetIncompatibleVersion()
    {
        return ObjectSettings<T>::OBJECT_INFO_DATA.INCOMPATIBLE;
    }

    template <typename T>
    consteval std::string_view GetObjectCategory()
    {
        return ObjectSettings<T>::OBJECT_INFO_DATA.CATEGORY;
    }

    template <typename T>
    consteval uintptr_t GetObjectRange()
    {
        return ObjectSettings<T>::OBJECT_INFO_DATA.RANGE;
    }


    template <typename T>
    consteval ObjectStorage GetObjectStorage()
    {
        if (is_complete_type_v<ObjectSettings<T>>) {
            constexpr ObjectStorage result = ObjectSettings<T>::OBJECT_INFO_DATA.STORAGE;

            if constexpr (result == ObjectStorage::Auto) {
                return detail::default_storage<T>;
            }
            else {
                return result;
            }
        }
        else {
            return detail::default_storage<T>;
        }
    }
}