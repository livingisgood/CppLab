#include "gtest/gtest.h"
#include "DataStructure/IntrusivieList.h"

using namespace BC;

namespace IntrusiveTest
{
    class FUnit
    {
    public:
        explicit FUnit(int InValue) : Value(InValue) {}
        int Value {0};

        Intrusive::TNode<FUnit> UnitLink {this};
    };

    using IntrusiveList = Intrusive::TNodeList<FUnit, &FUnit::UnitLink>;

    class FObjectList : public ::testing::Test
    {
    protected:
        virtual void SetUp() override
        {
            for(int i = 0; i < 10; ++i)
            {
                UnitVec.push_back(new FUnit(i));
            }
        }

        virtual void TearDown() override
        {
            for(auto UnitPtr : UnitVec)
            {
                if(UnitPtr != nullptr)
                    delete UnitPtr;
            }
        }

        std::vector<FUnit*> UnitVec;
    };

    TEST_F(FObjectList, PushBack)
    {
        IntrusiveList UnitList;

        for(auto UnitPtr : UnitVec)
        {
            UnitList.PushBack(*UnitPtr);
        }

        int i = 0;
        for(auto& Unit : UnitList)
        {
            EXPECT_EQ(&Unit, UnitVec[i]);
            ++i;
        }
    }

    TEST_F(FObjectList, PushFront)
    {
        IntrusiveList UnitList;

        for(auto UnitPtr : UnitVec)
        {
            UnitList.PushFront(*UnitPtr);
        }

        int i = 0;
        for(auto& Unit : UnitList)
        {
            EXPECT_EQ(&Unit, UnitVec[UnitVec.size() - 1 - i]);
            ++i;
        }
    }

    TEST_F(FObjectList, FrontAndBack)
    {
        IntrusiveList UnitList;

        for(auto UnitPtr : UnitVec)
        {
            UnitList.PushBack(*UnitPtr);
        }

        EXPECT_EQ(UnitVec[0], &UnitList.Front());
        EXPECT_EQ(UnitVec.back(), &UnitList.Back());
    }

    TEST_F(FObjectList, Erase)
    {
        IntrusiveList UnitList;

        for(auto UnitPtr : UnitVec)
        {
            UnitList.PushBack(*UnitPtr);
        }

        for(auto& Ptr : UnitVec)
        {
            if(Ptr->Value % 2 == 0)
            {
                delete Ptr;
                Ptr = nullptr;
            }
        }

        for(auto& Unit : UnitList)
        {
            EXPECT_TRUE(Unit.Value % 2 != 0);
        }
    }

    TEST_F(FObjectList, Erase2)
    {
        IntrusiveList UnitList;

        EXPECT_TRUE(UnitList.Empty());

        for(auto UnitPtr : UnitVec)
        {
            UnitList.PushBack(*UnitPtr);
        }

        for(auto It = UnitList.begin(); It != UnitList.end();)
        {
            if(It->Value % 2 == 0)
            {
                It = UnitList.Erase(It);
            }
            else
            {
                ++It;
            }
        }

        int i = 0;
        for(auto& Unit : UnitList)
        {
            EXPECT_EQ(Unit.Value, 2 * i + 1);
            ++i;
        }
    }

    TEST_F(FObjectList, Transfer)
    {
        IntrusiveList ListA;
        IntrusiveList ListB;

        for(auto UnitPtr : UnitVec)
        {
            ListA.PushBack(*UnitPtr);
        }

        ListB.PushBack(*UnitVec[0]);
        EXPECT_EQ(&ListA.Front(), UnitVec[1]);
    }

    TEST_F(FObjectList, AutoRelease)
    {
        {
            IntrusiveList UnitList;

            for(auto& Ptr : UnitVec)
            {
                EXPECT_TRUE(!Ptr->UnitLink.IsLinked());
                UnitList.PushBack(*Ptr);
            }

            for(auto& Ptr : UnitVec)
            {
                EXPECT_TRUE(Ptr->UnitLink.IsLinked());
            }
        }

        for(auto& Ptr : UnitVec)
        {
            EXPECT_TRUE(!Ptr->UnitLink.IsLinked());
        }

        std::list<int> A;
        A.end();
    }
}