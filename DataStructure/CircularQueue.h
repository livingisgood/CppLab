#pragma once
#include <cstdint>
#include <cstdlib>
#include "MemoryUtility.h"

template<typename T>
class TCircularQueue
{
public:

    using FAddr = uint32_t;
    using SizeType = int32_t;

    TCircularQueue() = default;

    ~TCircularQueue()
    {
        if(Heap != nullptr)
        {
            std::free(Heap);
        }
    }

    TCircularQueue(const TCircularQueue& Other)
    {
        Write = Other.Write;
        Read = Other.Read;

        Capacity = Other.Capacity;

    }

    TCircularQueue(TCircularQueue&& Other)
    {

    }

    void Clear()
    {

    }

private:

    FAddr Write {0};
    FAddr Read {0};

    SizeType Capacity {0};
    T* Heap {nullptr};

};