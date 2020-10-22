#pragma once

#include <memory>
#include <type_traits>
#include "prelude.hpp"


namespace rstd {

// Wrapper over std::unique_ptr
template <typename T>
class Box final {
private:
    std::unique_ptr<T> base;

    void assert_store() const {
        assert_(bool(base));
    }
    explicit Box(T *ptr) : base(ptr) {}

public:
    Box() = default;
    explicit Box(T &&v) : base(new T(std::move(v))) {}
    explicit Box(const T &v) : base(new T(v)) {}
    ~Box() = default;

    Box(Box &&) = default;
    Box &operator=(Box &&) = default;

    Box(const Box &) = delete;
    Box &operator=(const Box &) = delete;

    static Box _from_raw(T *ptr) {
        return Box(ptr);
    }

    T *_raw() {
        return base.get();
    }
    const T *_raw() const {
        return base.get();
    }
    T *raw() {
        assert_store();
        return base.get();
    }
    const T *raw() const {
        assert_store();
        return base.get();
    }
    
    T &_get() {
        return *base;
    }
    const T &_get() const {
        return *base;
    }
    T &get() {
        assert_store();
        return _get();
    }
    const T &get() const {
        assert_store();
        return _get();
    }

    T &operator*() {
        return get();
    }
    const T &operator*() const {
        return get();
    }
    T *operator->() {
        return &get();
    }
    const T *operator->() const {
        return &get();
    }

    void drop() {
        base = std::unique_ptr<T>();
    }

    T *_into_raw() {
        return base.release();
    }
    T *into_raw() {
        assert_store();
        return _into_raw();
    }

    operator bool() const {
        return bool(base);
    }

    template <typename U>
    Box<U> upcast() {
        return Box<U>::_from_raw(static_cast<U*>(this->into_raw()));
    }

private:
    template <typename U, typename X=std::enable_if_t<std::is_base_of_v<T, U>, void>>
    static Result<U *, T *> downcast_ptr(T *ptr) {
        U *dptr = dynamic_cast<U *>(ptr);
        if (dptr != nullptr) {
            return Ok(dptr);
        } else {
            return Err(ptr);
        }
    }
    template <typename U, typename X=std::enable_if_t<std::is_base_of_v<T, U>, void>>
    static Result<const U *, const T *> downcast_ptr(const T *ptr) {
        const U *dptr = dynamic_cast<const U *>(ptr);
        if (dptr != nullptr) {
            return Ok(dptr);
        } else {
            return Err(ptr);
        }
    }

public:
    template <typename U, typename X=std::enable_if_t<std::is_base_of_v<T, U>, void>>
    Result<U *, T *> downcast_ref() {
        return downcast_ptr<U>(this->raw());
    }
    template <typename U, typename X=std::enable_if_t<std::is_base_of_v<T, U>, void>>
    Result<const U *, const T *> downcast_ref() const {
        return downcast_ptr<U>(this->raw());
    }
    template <
        typename U,
        typename X=std::enable_if_t<std::is_base_of_v<T, U>, void>,
        typename R=Result<Box<U>, Box<T>>
    >
    R downcast() {
        return downcast_ptr<U>(this->into_raw()).match(
            [](U *dptr) { return R::Ok(Box<U>::_from_raw(dptr)); },
            [](T *ptr) { return R::Err(Box<T>::_from_raw(ptr)); }
        );
    }

    template <typename U>
    Box(Box<U> &&derived) : Box(derived.template upcast<T>()) {}
    template <typename U>
    Box &operator=(Box<U> &&derived) {
        return *this = derived.template upcast<T>();
    }

    template <typename U, typename X=std::enable_if_t<std::is_base_of_v<T, U>, void>>
    explicit Box(U &&v) :
        Box(Box<U>(std::move(v)).template upcast<T>())
    {}
    template <typename U, typename X=std::enable_if_t<std::is_base_of_v<T, U>, void>>
    explicit Box(const U &v) :
        Box(Box<U>(v).template upcast<T>())
    {}
};

} // namespace rstd
