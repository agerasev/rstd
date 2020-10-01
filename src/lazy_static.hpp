#pragma once

#include <rstd/prelude.hpp>
#include <atomic>
#include <type_traits>

namespace rstd {

// FIXME: In C++20 atomic pointer is not POD
static_assert(std::is_pod<std::atomic<const void *>>::value);

template <typename T, T (*F)()>
class LazyStatic {
private:
    std::atomic<const T *> content;

public:
    LazyStatic() = default;
    ~LazyStatic() = default;

    const T &operator*() {
        const T *ptr = content.load(std::memory_order_acquire);
        if (ptr == nullptr) {
            ptr = new T(F());
            content.store(ptr, std::memory_order_release);
        }
        return *ptr;
    }
    const T *operator->() {
        return &this->operator*();
    }

    void _clear() {
        const T *ptr = content.load(std::memory_order_acquire);
        if (ptr != nullptr) {
            content.store(nullptr, std::memory_order_release);
            delete ptr;
        }
    }
};

static_assert(std::is_pod<LazyStatic<int, nullptr>>::value);

} // namespace rstd


#define static_block_(name) \
    struct __static_block__##name##__struct { \
        __static_block__##name##__struct(); \
    } __static_block__##name##__instance; \
    __static_block__##name##__struct::__static_block__##name##__struct()

#define static_atexit_(name) \
    struct __static_atexit__##name##__struct { \
        ~__static_atexit__##name##__struct(); \
    } __static_atexit__##name##__instance; \
    __static_atexit__##name##__struct::~__static_atexit__##name##__struct()

#define lazy_static_(Type, name) \
    Type __##name##__create(); \
    ::rstd::LazyStatic<Type, __##name##__create> name; \
    static_atexit_(name##__destroyer) { \
        name._clear(); \
    } \
    Type __##name##__create()

#define extern_lazy_static_(Type, name) \
    extern Type __##name##__create(); \
    extern ::rstd::LazyStatic<Type, __##name##__create> name
