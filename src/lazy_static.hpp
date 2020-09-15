#include <core/prelude.hpp>
#include <atomic>
#include <type_traits>

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
    const T &operator->() {
        return this->operator*();
    }

    void _clear() {
        const T *ptr = content.load(std::memory_order_acquire);
        if (ptr != nullptr) {
            content.store(nullptr, std::memory_order_release);
            delete ptr;
        }
    }
};

int __lazy_static__create() { return 1; }
static_assert(std::is_pod<LazyStatic<int, __lazy_static__create>>::value);

#define lazy_static_(Type, name)\
    Type __lazy_static__##name##__create();\
    LazyStatic<Type, __lazy_static__##name##__create> name;\
    struct __lazy_static__##name##__Destroyer {\
        ~__lazy_static__##name##__Destroyer() {\
            name._clear();\
        }\
    } __lazy_static__##name##__destroyer;\
    Type __lazy_static__##name##__create()
