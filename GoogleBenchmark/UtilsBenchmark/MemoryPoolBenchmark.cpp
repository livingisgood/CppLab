#include <benchmark/benchmark.h>
#include "MemoryPool.h"

using namespace BC;

struct FSmallStruct
{
    int A;
    int B;
    void* C;

    FSmallStruct(int InA, int InB, void* InC) : A(InA), B(InB), C(InC) {}
};

struct FMediumStruct
{
    int A[20];
};

struct FLargeStruct
{
    int A[500];
};

void RawNewDelete(int Times)
{
    std::vector<FSmallStruct*> V0(Times);
    std::vector<FMediumStruct*> V1(Times);
    std::vector<FLargeStruct*> V2(Times);

    for(int i = 0; i < Times; ++i)
    {
        V0[i] = new FSmallStruct(1, 2, nullptr);
        V1[i] = new FMediumStruct;
        V2[i] = new FLargeStruct;
    }

    for(int i = 0; i < Times; ++i)
    {
        delete V0[i];
        delete V1[i];
        delete V2[i];
    }
}

void PoolRecycle(FMemoryPool& Pool, int Times)
{
    std::vector<FSmallStruct*> V0(Times);
    std::vector<FMediumStruct*> V1(Times);
    std::vector<FLargeStruct*> V2(Times);

    for(int i = 0; i < Times; ++i)
    {
        V0[i] = Pool.Construct<FSmallStruct>(1, 2, nullptr);
        V1[i] = Pool.Construct<FMediumStruct>();
        V2[i] = Pool.Construct<FLargeStruct>();
    }

    for(int i = 0; i < Times; ++i)
    {
        Pool.Destruct(V0[i]);
        Pool.Destruct(V1[i]);
        Pool.Destruct(V2[i]);
    }
}

static void BM_DynamicNewDelete(benchmark::State& state)
{
    for(auto _ : state)
    {
        RawNewDelete(10);
    }
}

BENCHMARK(BM_DynamicNewDelete);

static void BM_PoolRecycle(benchmark::State& state)
{
    FMemoryPool Pool;

    for(auto _ : state)
    {
        PoolRecycle(Pool, 10);
    }
}

BENCHMARK(BM_PoolRecycle);

BENCHMARK_MAIN();