#include "gtest/gtest.h"
#include "Utils/Event.h"
#include <iostream>

using namespace BC;

namespace EventTest
{
    void Printer(int Value)
    {
        std::cout << "Print " << Value << std::endl;
    }

    class FCaller
    {
    public:

        void Bind(TEvent<int>& Event)
        {
            Keeper.Keep(Event.Subscribe(this, &FCaller::CallBackA));
            Keeper.Keep(Event.Subscribe(this, &FCaller::CallBackB));
        }

    private:

        void CallBackA(int V)
        {
            std::cout << "Callback A " << V << std::endl;
        }

        void CallBackB(int V)
        {
            std::cout << "CallBack B " << V << std::endl;
        }

        FListenerKeeper Keeper;
    };

    TEST(EventTest, GlobalSub)
    {
        TEvent<int> MyEvent;

        MyEvent.Subscribe(Printer);
        MyEvent(5);

        MyEvent.Clear();
        MyEvent(4);
    }

    TEST(EventTest, GlobalSub2)
    {
        TEvent<int> MyEvent;

        {
            FListenerKeeper Keeper;
            Keeper.Keep(MyEvent.Subscribe(Printer));
            MyEvent.Invoke(5);
        }

        MyEvent.Invoke(4);
    }

    TEST(EventTest, AutoRelease)
    {
        TEvent<int> MyEvent;

        {
            FCaller Caller;
            Caller.Bind(MyEvent);
            MyEvent(3);
        }
        MyEvent(3);
    }

    TEST(EventTest, AutoReleaseEvent)
    {
        FCaller Caller;
        {
            TEvent<int> MyEvent;
            Caller.Bind(MyEvent);
        }
    }
}
