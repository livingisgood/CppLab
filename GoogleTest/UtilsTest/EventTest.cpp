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

    TEST(EventTest, GlobalSub)
    {
        TEvent<int> MyEvent;

        MyEvent.Subscribe(Printer);
        MyEvent(5);
    }
}
