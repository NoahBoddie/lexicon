#pragma once

namespace LEX
{
    template <class T> requires(requires (T* ptr) { { ptr->Destroy() } -> std::same_as<void>; })
        struct destroy_delete { // default deleter for unique_ptr
        constexpr destroy_delete() noexcept = default;

        template <class Ty, std::enable_if_t<std::is_convertible_v<Ty*, T*>, int> = 0>
        constexpr destroy_delete(const destroy_delete<Ty>&) noexcept {}

        constexpr void operator()(T* ptr) const noexcept {
            static_assert(0 < sizeof(T), "can't delete an incomplete type");
            ptr->Destroy();
        }
    };
    template <typename T>
    using destructible_ptr = std::unique_ptr<T, destroy_delete<T>>;
}