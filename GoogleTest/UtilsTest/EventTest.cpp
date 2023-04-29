#include "gtest/gtest.h"
#include "Utils/Event.h"

using namespace BC;

class FooSub
{
public:

    void Trigger()
    {
        ++Value;
    }

    void Trigger() const
    {
        ++MutableValue;
    }

    int Value {0};
    mutable int MutableValue {0};
};

TEST(Event, DoubleSubscribe)
{
    TEvent MyEvent;

    FooSub* Caller = new FooSub;

    MyEvent.Subscribe(Caller, &FooSub::Trigger);
    MyEvent.Subscribe(Caller, &FooSub::Trigger);
    MyEvent.Subscribe((const FooSub*)Caller, &FooSub::Trigger);

    MyEvent();

    EXPECT_EQ(Caller->Value, 2);
    EXPECT_EQ(Caller->MutableValue, 1);

    delete Caller;
}

TEST(Event, Unsubscribe)
{
    TEvent MyEvent;

    FooSub* Caller = new FooSub;

    MyEvent.Subscribe(Caller, &FooSub::Trigger);
    MyEvent.Subscribe(Caller, &FooSub::Trigger);

    MyEvent();

    EXPECT_EQ(Caller->Value, 2);

    MyEvent.UnSubscribe(Caller);

    MyEvent();
    EXPECT_EQ(Caller->Value, 2);

    FListenerHandle HandleA = MyEvent.Subscribe(Caller, &FooSub::Trigger);
    FListenerHandle HandleB = MyEvent.Subscribe(Caller, &FooSub::Trigger);

    MyEvent();
    EXPECT_EQ(Caller->Value, 4);

    MyEvent.UnSubscribe(HandleA);

    MyEvent();
    EXPECT_EQ(Caller->Value, 5);

    MyEvent.UnSubscribe(HandleA);

    MyEvent();
    EXPECT_EQ(Caller->Value, 6);

    MyEvent.UnSubscribe(HandleB);
    MyEvent();
    EXPECT_EQ(Caller->Value, 6);

    delete Caller;
}

