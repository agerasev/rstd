#pragma once

#include <functional>
#include <limits>
#include <type_traits>

#include <core/option.hpp>

namespace core::misc {

template <typename Dst, typename Src>
Option<std::reference_wrapper<Dst>> downcast(Src &src) {
    static_assert(std::is_base_of_v<Src, Dst>);
    static_assert(std::is_convertible_v<Dst *, Src *>);

    Dst *ptr = dynamic_cast<Dst *>(&src);
    if (ptr != nullptr) {
        return Some(std::ref(*ptr));
    }
    return None();
}

} // namespace core::misc
