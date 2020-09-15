#include <core/prelude.hpp>
#include <atomic>
#include <type_traits>

static_assert(std::is_pod<std::atomic<bool>>::value);

template <typename T, typename F>
class LazyStatic {
private:
    std::atomic<bool> initialized;
    const T *object;
public:
    LazyStatic() = default;
    ~LazyStatic() = default;

    void destroy() {
        delete object;
    }
};
static_assert(std::is_pod<LazyStatic<void, void(*)()>>::value);

#define lazy_static_(Type, name, value) \
    LazyStatic name;

