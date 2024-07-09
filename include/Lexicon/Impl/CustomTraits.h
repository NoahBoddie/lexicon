#pragma once

namespace LEX
{

    template<typename T>
    concept numeric = std::is_integral_v<T> || std::is_floating_point_v<T>;

}
