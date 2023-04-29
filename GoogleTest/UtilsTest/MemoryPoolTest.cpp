#include "gtest/gtest.h"
#include "MemoryPool.h"
#include <iostream>

using namespace BC;

bool TestBlockSize(std::size_t BlockSize)
{
    std::size_t One = 1;
    return (One << FMemoryPool::GetSizeIndex(BlockSize)) >= BlockSize;
}

TEST(MemoryPool, SizeIndex)
{
    EXPECT_TRUE(TestBlockSize(0));
    EXPECT_TRUE(TestBlockSize(1));
    EXPECT_TRUE(TestBlockSize(2));
    EXPECT_TRUE(TestBlockSize(1023));
    EXPECT_TRUE(TestBlockSize(1024));
    EXPECT_TRUE(TestBlockSize(1025));

    EXPECT_EQ(FMemoryPool::GetSizeIndex(0), 0);
    EXPECT_EQ(FMemoryPool::GetSizeIndex(1), 0);
    EXPECT_EQ(FMemoryPool::GetSizeIndex(2), 1);
    EXPECT_EQ(FMemoryPool::GetSizeIndex(1023), 10);
    EXPECT_EQ(FMemoryPool::GetSizeIndex(1024), 10);
    EXPECT_EQ(FMemoryPool::GetSizeIndex(1025), 11);
}

struct Foo
{
    int A;
    explicit Foo(int InA) : A(InA)
    {
        std::cout << "Foo " << A << " is constructed." << std::endl;
    }

    ~Foo()
    {
        std::cout << "Foo " << A << " is destructed." << std::endl;
    }
};

TEST(MemoryPool, DoubleDestruct)
{
    FMemoryPool Pool;

    Foo* Obj = Pool.Construct<Foo>(3);
    Pool.Destruct(Obj);
    Pool.Destruct(Obj);
}