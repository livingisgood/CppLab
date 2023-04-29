#include "gtest/gtest.h"
#include "SmallVector.h"
#include <vector>
#include "ContainerInserter.h"

#define SUITE SmallVector

using namespace BC;

template <typename T, int N>
void TestEqual(const std::vector<T>& StdVector, const TSmallVector<T, N>& SmallVector)
{
    ASSERT_EQ(StdVector.size(), SmallVector.GetSize());

    for(int i = 0; i < SmallVector.GetSize(); ++i)
    {
        ASSERT_EQ(StdVector[i], SmallVector[i]);
    }
}

void AddTen(FContainerInserter<int>& Inserter)
{
    for(int i = 0; i < 10; ++i)
    {
        Inserter.Insert(i);
    }
}

void TestInserter()
{
    std::vector<int> VecA;
    FContainerInserterImpl<decltype(VecA)> Inserter(VecA);

    AddTen(Inserter);
    for(auto Item : VecA)
    {
        std::cout << Item << ",";
    }
    std::cout << std::endl;

    TSmallVector<int, 4> VecB;
    FContainerInserterImpl<decltype(VecB)> Inserter2(VecB);

    AddTen(Inserter2);
    for(auto Item : VecB)
    {
        std::cout << Item << ",";
    }
    std::cout << std::endl;

    FPrinter<int> Printer;
    AddTen(Printer);

    FAccumulator<int> Accumulator;
    AddTen(Accumulator);

    std::cout << std::endl << Accumulator.GetSum() << std::endl;
}

TEST(SUITE, Inserter)
{
    TestInserter();
}

template <typename T>
class TMirror
{
public:

    using ConstIt = typename TSmallVector<T, 10>::ConstIterator;
    using It = typename TSmallVector<T, 10>::Iterator;

    void Assign(std::initializer_list<T> InitList)
    {
        StdVec = InitList;
        SmallVec = InitList;
    }

    void Init(const std::vector<T>& Other)
    {
        StdVec = Other;
        SmallVec.Init(Other.data(), Other.size());
    }

    void Check()
    {
        TestEqual(StdVec, SmallVec);
    }

    void Add(const T& Item)
    {
        StdVec.push_back(Item);
        SmallVec.Add(Item);
    }

    template<typename... ArgsType>
    void Emplace(int Index, ArgsType&&... Args)
    {
        SmallVec.Emplace(SmallVec.begin() + Index, std::forward<ArgsType>(Args)...);
        StdVec.emplace(StdVec.begin() + Index, std::forward<ArgsType>(Args)...);
    }

    void Insert(int Index, const T& Value)
    {
        SmallVec.Insert(SmallVec.begin() + Index, Value);
        StdVec.insert(StdVec.begin() + Index, Value);
    }

    void Insert(int Index, const T* First, int Count)
    {
        SmallVec.Insert(SmallVec.begin() + Index, First, Count);
        StdVec.insert(StdVec.begin() + Index, First, First + Count);
    }

    template<typename InputIt>
    void Insert(int Index, InputIt First, InputIt Last)
    {
        SmallVec.Insert(SmallVec.begin() + Index, First, Last);
        StdVec.insert(StdVec.begin() + Index, First, Last);
    }

    void Erase(int Index)
    {
        SmallVec.Erase(SmallVec.begin() + Index);
        StdVec.erase(StdVec.begin() + Index);
    }

    void Erase(int First, int Last)
    {
        SmallVec.Erase(SmallVec.begin() + First, SmallVec.begin() + Last);
        StdVec.erase(StdVec.begin() + First, StdVec.begin() + Last);
    }

    std::vector<T> StdVec;
    TSmallVector<T, 10> SmallVec;
};

class MirrorTest : public ::testing::Test
{
protected:

    void SetUp() override
    {
        std::vector<int> InitVec;
        for(int i = 0; i < 20; ++i)
        {
            InitVec.push_back(i);
        }

        Mirror.Init(InitVec);
    }

    TMirror<int> Mirror;
};

TEST(SUITE, DefaultConstructor)
{
    TSmallVector<int, 5> Vec;

    EXPECT_EQ(Vec.GetCapacity(), 5);
    EXPECT_EQ(Vec.GetSize(), 0);
}

TEST(SUITE, MoveConstructor)
{
    TSmallVector<int, 5> VecB = { 1, 2, 3, 4, 5, 6, 7 };
    auto HeapB = VecB.GetHeap();
    auto SizeB = VecB.GetSize();

    EXPECT_NE(HeapB, nullptr);

    TSmallVector<int, 5> VecC = { 1, 2, 3 };
    EXPECT_EQ(VecC.GetHeap(), nullptr);

    TSmallVector<int, 5> VecA(std::move(VecB));
    EXPECT_EQ(VecB.GetHeap(), nullptr);
    EXPECT_EQ(VecA.GetHeap(), HeapB);
    EXPECT_EQ(VecA.GetSize(), SizeB);

    TSmallVector<int, 5> VecD(std::move(VecC));
    EXPECT_EQ(VecD.GetCapacity(), 5);
    EXPECT_EQ(VecD.GetHeap(), nullptr);

    TSmallVector<int, 10> VecE = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    TSmallVector<int, 10> VecF = {1, 2, 3, 4, 5, 6};
    int VecFSize = VecF.GetSize();
    VecE = std::move(VecF);

    EXPECT_EQ(VecE.GetSize(), VecFSize);
    EXPECT_EQ(VecE.GetHeap(), nullptr);

    std::vector<int> StdVec = {1,2,3,4,5,6};
    TestEqual(StdVec, VecE);
}

TEST(SUITE, DataLayout)
{
    TSmallVector<int, 5> Vec;

    for(int i = 0; i < 5; ++i)
    {
        EXPECT_EQ(Vec.GetSize(), i);
        Vec.Add(i);
        EXPECT_EQ(Vec.GetHeap(), nullptr);
    }

    Vec.Add(5);
    EXPECT_NE(Vec.GetHeap(), nullptr);
}

TEST(SUITE, BackInsert)
{
    TSmallVector<int, 5> Vec;

    std::back_insert_iterator<decltype(Vec)> Inserter(Vec);

    for(int i = 0; i < 5; ++i)
    {
        Inserter = i;
        EXPECT_EQ(i, Vec[i]);
    }
}

TEST(SUITE, UninitializedAdd)
{
    TSmallVector<int, 5> Vec;

    AddUninitialized(Vec, 3);

    EXPECT_EQ(Vec.GetSize(), 3);

    for(int i = 0; i < 3; ++i)
    {
        Vec[i] = i;
    }
}

TEST(SUITE, ReserveTest)
{
    std::vector<int> StdVec = {1, 2, 3};
    TSmallVector<int, 3> SmallVec = {1, 2, 3};

    SmallVec.Reserve(10);
    TestEqual(StdVec, SmallVec);

    std::vector<int> StdVec2 = {1, 2, 3, 4, 5};
    TSmallVector<int, 3> SmallVec2;
    SmallVec2.Insert(SmallVec2.begin(), StdVec2.begin(), StdVec2.end());

    TestEqual(StdVec2, SmallVec2);
    SmallVec2.Reserve(100);
    TestEqual(StdVec2, SmallVec2);
}

class Foo
{
public:

    static int RefCount;

    Foo()
    {
        ++RefCount;
    }

    Foo(const Foo& Other)
    {
        ++RefCount;
    }

    Foo(Foo&& Other) noexcept
    {
        ++RefCount;
    }

    Foo& operator=(const Foo& Other) = default;

    Foo& operator=(Foo&& Other) = default;

    ~Foo()
    {
        --RefCount;
    }
};

int Foo::RefCount = 0;

TEST(SUITE, DataDestruction)
{
    EXPECT_EQ(Foo::RefCount, 0);

    auto FooList = new TSmallVector<Foo, 3>;

    FooList->Add(Foo());
    EXPECT_EQ(Foo::RefCount, 1);

    delete FooList;

    EXPECT_EQ(Foo::RefCount, 0);

    FooList = new TSmallVector<Foo, 3>;

    for(int i = 0; i < 10; ++i)
    {
        FooList->EmplaceBack();
    }

    EXPECT_EQ(Foo::RefCount, 10);

    FooList->Erase(FooList->begin(), FooList->begin() + 5);
    EXPECT_EQ(Foo::RefCount, 5);

    FooList->Add(Foo());
    EXPECT_EQ(Foo::RefCount, 6);

    FooList->Insert(FooList->end(), 100, Foo());
    EXPECT_EQ(Foo::RefCount, 106);

    delete FooList;
    EXPECT_EQ(Foo::RefCount, 0);
}

TEST_F(MirrorTest, Init)
{
    Mirror.Check();
}

TEST_F(MirrorTest, Assign)
{
    Mirror.Assign({1, 2, 3, 4, 5, 6});
    Mirror.Check();
}

TEST_F(MirrorTest, Copy)
{
    TSmallVector<int, 10> Copied(Mirror.SmallVec);

    TestEqual(Mirror.StdVec, Copied);
}

TEST_F(MirrorTest, Misc)
{
    Mirror.Add(0);
    Mirror.Check();

    Mirror.Emplace(0, 100);
    Mirror.Check();

    Mirror.Insert(10, 99);
    Mirror.Check();

    std::vector<int> Temp = { 8, 8, 8, 7, 7, 7, 7, 7 };
    Mirror.Insert(static_cast<int>(Mirror.StdVec.size()), Temp.data(), static_cast<int>(Temp.size()));
    Mirror.Check();

    Mirror.Erase(0, 10);
    Mirror.Check();

    Mirror.Erase(0);
    Mirror.Check();

    Mirror.Insert(0, Temp.begin(), Temp.end());
    Mirror.Check();
}

