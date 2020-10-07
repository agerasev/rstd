#pragma once

#include <pthread.h>
#include <core/once.hpp>
#include <rstd/prelude.hpp>


template <typename T, void(*F)()>
class LazyStatic {
private:
    core::Once once;
    const T *content = nullptr;

public:
    void __init(T &&x) {
        assert_(content == nullptr);
        content = new T(std::move(x));
    }
    void __clear() {
        if (content != nullptr) {
            delete content;
            content = nullptr;
        }
    }

public:
    LazyStatic() = default;
    ~LazyStatic() = default;

    LazyStatic(const LazyStatic &) = delete;
    LazyStatic &operator=(const LazyStatic &) = delete;

    const T &operator*() {
        once.call_once(F);
        return *content;
    }
    const T *operator->() {
        return &this->operator*();
    }
};

// FIXME: Make LazyStatic to be a POD
//static_assert(std::is_pod<LazyStatic<int, nullptr>>::value);


#define __static_block_(prefix, name) \
    prefix struct __static_block__##name##__struct { \
        __static_block__##name##__struct(); \
    } __static_block__##name##__instance; \
    __static_block__##name##__struct::__static_block__##name##__struct()

#define __static_atexit_(prefix, name) \
    prefix struct __static_atexit__##name##__struct { \
        ~__static_atexit__##name##__struct(); \
    } __static_atexit__##name##__instance; \
    __static_atexit__##name##__struct::~__static_atexit__##name##__struct()

#define static_block_(name) \
    __static_block_(, name)

#define static_atexit_(name) \
    __static_atexit_(, name)

#define __lazy_static_(prefix, Type, name) \
    prefix Type __##name##__create(); \
    prefix void __##name##__init(); \
    prefix ::LazyStatic<Type, __##name##__init> name; \
    prefix void __##name##__init() { \
        name.__init(__##name##__create()); \
    } \
    __static_atexit_(prefix, name##__destroyer) { \
        name.__clear(); \
    } \
    prefix Type __##name##__create()

#define lazy_static_(Type, name) \
    __lazy_static_(, Type, name) \

#define static_lazy_static_(Type, name) \
    __lazy_static_(static, Type, name) \

#define extern_lazy_static_(Type, name) \
    extern void __##name##__init(); \
    extern ::LazyStatic<Type, __##name##__init> name
