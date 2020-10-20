#include <rtest.hpp>
#include <memory>
#include "box.hpp"

using namespace rstd;


rtest_module_(box) {
    class Base {
    public:
        virtual ~Base() = default;
        virtual int foo() = 0;
        //virtual Box<Base> copy() const = 0;
    };
    class One : public Base {
    public:
        int val = 123;
        explicit One(int v) : val(v) {}
        virtual int foo() override {
            return this->val;
        }
        //virtual Box<One> copy() const override {
        //    return Box(*this);
        //}
    };
    class Two : public Base {
    public:
        double val = 0.0;
        explicit Two(double v) : val(v) {}
        virtual int foo() override {
            return int(this->val);
        }
        //virtual Box<Two> copy() const override {
        //    return Box(*this);
        //}
    };

    rtest_(move) {
        std::unique_ptr<int> ptr(new int(123));
        assert_eq_(*ptr, 123);

        auto box = Box(std::move(ptr));
        assert_(!bool(ptr));
        assert_(bool(box));
        assert_eq_(**box, 123);
    }
    rtest_should_panic_(empty) {
        Box<int> box;
        *box;
    }
    rtest_(upcast) {
        One derived(123);
        assert_eq_(derived.foo(), 123);

        Box<One> dbox(std::move(derived));
        assert_eq_(dbox->foo(), 123);

        Box<Base> bbox = std::move(dbox);
        assert_(!bool(dbox));
        assert_eq_(bbox->foo(), 123);
    }
    rtest_(downcast_raw) {
        Base *base = new One(123);
        assert_eq_(base->foo(), 123);

        Two *two = dynamic_cast<Two *>(base);
        assert_(two == nullptr);

        One *one = dynamic_cast<One *>(base);
        assert_(one != nullptr);
        assert_eq_(one->foo(), 123);
    }
    rtest_(downcast) {
        Box<Base> box = Box(One(123));
        assert_eq_(box->foo(), 123);

        box = box.downcast<Two>().unwrap_err();
        assert_eq_(box->foo(), 123);

        Box<One> dbox = box.downcast<One>().unwrap();
        assert_eq_(dbox->foo(), 123);
    }
}
