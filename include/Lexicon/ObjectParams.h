#pragma once

#include "Lexicon/ObjectData.h"

namespace LEX
{
    struct Object;

    struct ObjectParams
    {
        ObjectParams(const Object& obj);

        constexpr ObjectParams(ObjectData d, ObjectDataType t, std::optional<uint16_t> c) noexcept :
            data{ d }, type{ t }, context{ c } {
        }
    


        mutable ObjectData data{};
        ObjectDataType type = ObjectDataType::kNone;
        std::optional<uint16_t> context{};


        void CheckValid() const
        {
            assert_if(type == ObjectDataType::kNone) {
                report::runtime::error("Cannot retrieve data of empty ObjectData.");
            }
        }

        operator bool() const noexcept
        {
            return type != ObjectDataType::kNone;
        }


        //Make an empty or function.
        template <typename T>
        decltype(auto) get()
        {
            CheckValid();
            return data.get<T>();
        }

        template <typename T>
        decltype(auto) get() const
        {
            CheckValid();
            return data.get<T>();
        }


        template <typename T>
        auto* ptr()
        {
            CheckValid();
            return data.ptr<T>();
        }

        template <typename T>
        const auto* ptr() const
        {
            CheckValid();
            return data.ptr<T>();
        }
    };


}