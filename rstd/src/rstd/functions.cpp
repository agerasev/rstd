#include <rtest.hpp>

#include "functions.hpp"

using namespace rstd;


rtest_module_(functions) {
    struct Action {
        inline static const int
            NONE = 0,
            COPY = 1,
            MOVE = 2;
    };
    class Recorder {
    public:
        int history = Action::NONE;
        Recorder() = default;
        Recorder(const Recorder &o) : history(o.history | Action::COPY) {}
        Recorder &operator=(const Recorder &o) { history = o.history | Action::COPY; return *this; }
        Recorder(Recorder &&o) : history(o.history | Action::MOVE) {}
        Recorder &operator=(Recorder &&o) { history = o.history | Action::MOVE; return *this; }
    };

    rtest_(move) {
        auto x = Recorder();
        assert_eq_(x.history, Action::NONE);
        auto y = move(x);
        assert_eq_(y.history, Action::MOVE);
        auto z = move(Recorder());
        assert_eq_(z.history, Action::MOVE);
    }
}
