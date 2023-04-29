#include <benchmark/benchmark.h>
#include "SmallVector.h"

using namespace BC;

struct FPoint
{
    float X;
    float Y;
};

static void BM_std(benchmark::State& state)
{
    for(auto _ : state)
    {
        std::vector<FPoint> Points;
        Points.reserve(5);
        for(int i = 0; i < 5; ++i)
        {
            Points.push_back(FPoint());
        }
    }
}

BENCHMARK(BM_std);

static void BM_SmallVec(benchmark::State& state)
{
    for(auto _ : state)
    {
        TSmallVector<FPoint, 5> Points;
        for(int i = 0; i < 5; ++i)
        {
            Points.Add(FPoint());
        }
    }
}

BENCHMARK(BM_SmallVec);

//BENCHMARK_MAIN();