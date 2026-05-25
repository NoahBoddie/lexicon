#pragma once

namespace LEX
{
    ENUM(ObjectStorage, uint8_t)
    {
        Auto,
        Value,
        Pointer,
    };

    namespace detail
    {
        template <typename T>
        constexpr ObjectStorage default_storage = !std::is_polymorphic_v<T> && std::is_trivially_copyable_v<T> && sizeof(T) <= (sizeof(void*)) ?
            ObjectStorage::Value : ObjectStorage::Pointer;
    }
}