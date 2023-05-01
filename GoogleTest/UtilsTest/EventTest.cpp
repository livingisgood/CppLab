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

    class FFoo
    {
    public:

        ~FFoo()
        {
            if(Handle)
                Handle->Unsubscribe();
        }

        void BindEvent(TEvent<int>& Event)
        {
            Handle = Event.Subscribe(this, &FFoo::HandleInput);
        }

    private:

        void HandleInput(int Value)
        {
            std::cout << "Print " << Value << std::endl;
        }

        TEvent<int>::FHandle Handle;
    };

    TEST(EventTest, GlobalSub)
    {
        TEvent<int> MyEvent;

        MyEvent.Subscribe(Printer);
        MyEvent(5);
    }

    TEST(EventTest, GlobalSub2)
    {
        TEvent<int> MyEvent;

        TEvent<int>::FHandle Handle = MyEvent.Subscribe(Printer);
        MyEvent.Invoke(3);

        Handle->Unsubscribe();
        Handle->Unsubscribe();
        MyEvent.Invoke(2);
    }

    TEST(EventTest, unsub)
    {
        std::cout << "unsub" << std::endl;

        TEvent<int> MyEvent;

        FFoo* Foo = new FFoo;
        Foo->BindEvent(MyEvent);

        MyEvent(3);
        delete Foo;

        MyEvent(2);
    }
}
