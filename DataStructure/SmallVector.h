#pragma once
#include <cstring>
#include <iterator>
#include <algorithm>
#include <memory>
#include "MemoryUtility.h"

namespace BC
{
    template<typename T, int N>
    class TSmallVector
    {
        static_assert(N >= 0, "Internal Storage Num has to be non-negative");

    public:

        using Iterator = T*;
        using ConstIterator = const T*;
        using DifferenceType = std::intptr_t;

        TSmallVector() = default;

        ~TSmallVector()
        {
            Dispose();
        }

        TSmallVector(const TSmallVector& Other)
        {
            Init(Other.GetData(), Other.Size);
        }

        TSmallVector(TSmallVector&& Other) noexcept
        {
            MoveFrom(std::forward<TSmallVector<T, N>>(Other));
        }

        TSmallVector& operator=(const TSmallVector& Other)
        {
            if(this != &Other)
            {
                Init(Other.GetData, Other.Size);
            }
            return *this;
        }

        TSmallVector& operator=(TSmallVector&& Other) noexcept
        {
            if(this != &Other)
            {
                MoveFrom(std::forward<TSmallVector<T, N>>(Other));
            }
            return *this;
        }

        TSmallVector(std::initializer_list<T> InitList)
        {
            Insert(begin(), InitList.begin(), InitList.end());
        }

        TSmallVector& operator=(std::initializer_list<T> InitList)
        {
            Clear();
            Insert(begin(), InitList.begin(), InitList.end());
            return *this;
        }

        const T* GetData() const
        {
            return HeapData == nullptr ? reinterpret_cast<const T*>(Storage) : HeapData;
        }

        T* GetData()
        {
            return HeapData == nullptr ? reinterpret_cast<T*>(Storage) : HeapData;
        }

        const T* GetHeap() const
        {
            return HeapData;
        }

        int GetSize() const
        {
            return Size;
        }

        bool IsEmpty() const { return Size == 0;  }

        int GetCapacity() const
        {
            return Capacity;
        }

        T& operator[](int Index)
        {
            return GetData()[Index];
        }

        const T& operator[](int Index) const
        {
            return GetData()[Index];
        }

        Iterator begin() { return GetData(); }
        Iterator end() { return GetData() + Size;  }

        ConstIterator begin() const { return GetData(); }
        ConstIterator end() const { return GetData() + Size; }

        void Reserve(int RequiredCapacity)
        {
            if(RequiredCapacity <= Capacity)
                return;

            if(std::is_trivially_copyable<T>::value)  // in cpp17 we can use if constexpr instead of a runtime code path
            {
                Capacity = CalculateCapacityGrowth(RequiredCapacity);
                if(HeapData == nullptr)
                {
                    HeapData = static_cast<T*>(std::malloc(sizeof(T) * Capacity));
                    std::memcpy(HeapData, Storage, sizeof(T) * Size);
                }
                else
                {
                    HeapData = static_cast<T*>(std::realloc(HeapData, sizeof(T) * Capacity));
                }
            }
            else
            {
                int NewCapacity = CalculateCapacityGrowth(RequiredCapacity);
                T* NewHeap = static_cast<T*>(std::malloc(sizeof(T) * NewCapacity));

                UninitializedTransferN<T>(GetData(), Size, NewHeap);

                if(Size > 0)
                    DestroyRange(GetData(), GetData() + Size);

                SetHeap(NewHeap);
                Capacity = NewCapacity;
            }
        }

        void Init(const T* Data, int DataNum)
        {
            if(DataNum <= 0)
            {
                Clear();
                return;
            }

            if(DataNum <= Size)
            {
                T* MyFirst = begin();
                T* MyLast = end();

                std::copy_n(Data, DataNum, MyFirst);
                DestroyRange(MyFirst + DataNum, MyLast);
                Size = DataNum;
                return;
            }

            if(DataNum <= Capacity)
            {
                T* MyFirst = begin();
                T* MyLast = end();

                std::copy_n(Data, Size, MyFirst);
                std::uninitialized_copy_n(Data + Size, DataNum - Size, MyLast);
                Size = DataNum;
                return;
            }

            ClearAndReserve(DataNum);
            std::uninitialized_copy_n(Data, DataNum, GetData());
            Size = DataNum;
        }

        void Clear()
        {
            if(Size > 0)
            {
                T* MyFirst = GetData();
                T* MyLast = MyFirst + Size;

                DestroyRange(MyFirst, MyLast);
                Size = 0;
            }
        }

        void Add(const T& Item)
        {
            T* InsertPos = MakeRoom(end(), 1);
            new (InsertPos) T(Item);
        }

        void Add(T&& Item)
        {
            T* InsertPos = MakeRoom(end(), 1);
            new (InsertPos) T(std::forward<T>(Item));
        }

        template<typename... ArgsType>
        T& EmplaceBack(ArgsType&&... Args)
        {
            T* InsertPos = MakeRoom(end(), 1);
            new (InsertPos) T(std::forward<ArgsType>(Args)...);

            T& Result = *InsertPos;
            return Result;
        }

        template<typename... ArgsType>
        T& Emplace(ConstIterator Pos, ArgsType&&... Args)
        {
            T* InsertPos = MakeRoom(Pos, 1);
            new (InsertPos) T(std::forward<ArgsType>(Args)...);

            T& Result = *InsertPos;
            return Result;
        }

        Iterator Insert(ConstIterator Pos, const T& Value)
        {
            T* InsertPos = MakeRoom(Pos, 1);
            new (InsertPos) T(Value);
            return InsertPos;
        }

        Iterator Insert(ConstIterator Pos, T&& Value)
        {
            T* InsertPos = MakeRoom(Pos, 1);
            new (InsertPos) T(std::move(Value));
            return InsertPos;
        }

        Iterator Insert(ConstIterator Pos, int Count, const T& Value)
        {
            T* InsertPos = MakeRoom(Pos, Count);
            std::uninitialized_fill_n(InsertPos, Count, Value);
            return InsertPos;
        }

        Iterator Insert(ConstIterator Pos, const T* First, int Count)
        {
            T* InsertPos = MakeRoom(Pos, Count);
            std::uninitialized_copy_n(First, Count, InsertPos);
            return InsertPos;
        }

        template<typename InputIt>
        Iterator Insert(ConstIterator Pos, InputIt First, InputIt Last)
        {
            DifferenceType Count = std::distance(First, Last);
            T* InsertPos = MakeRoom(Pos, Count);
            std::uninitialized_copy(First, Last, InsertPos);
            return InsertPos;
        }

        Iterator Erase(ConstIterator Pos)
        {
            T* ErasePos = const_cast<T*>(Pos);

            if(Pos == end())
                return ErasePos;

            std::move(ErasePos + 1, end(), ErasePos);
            DestroyObject(*(GetData() + Size - 1));
            --Size;
            return ErasePos;
        }

        Iterator Erase(ConstIterator First, ConstIterator Last)
        {
            T* ErasePos = const_cast<T*>(First);

            if(First == Last)
                return ErasePos;

            DifferenceType Count = std::distance(First, Last);
            std::move(ErasePos + Count, end(), ErasePos);
            DestroyRange(end() - Count, end());

            Size -= Count;
            return ErasePos;
        }

    private:

        void Dispose()
        {
            Clear();

            SetHeap(nullptr);
            Size = 0;
            Capacity = N;
        }

        void ClearAndReserve(int NewCapacity)
        {
            Clear();

            SetHeap(nullptr);
            Size = 0;
            Capacity = N;

            if(NewCapacity > N)
            {
                Capacity = CalculateCapacityGrowth(NewCapacity);
                HeapData = static_cast<T*>(std::malloc(Capacity * sizeof(T)));
            }
        }

        void MoveFrom(TSmallVector<T, N>&& Other)
        {
            Dispose();

            Size = Other.Size;
            Capacity = Other.Capacity;

            if(Other.HeapData != nullptr)
            {
                HeapData = Other.HeapData;
                Other.Size = 0;
                Other.Capacity = N;
                Other.HeapData = nullptr;
            }
            else
            {
                std::move(Other.Storage, Other.Storage + Size, Storage);
            }
        }

        int CalculateCapacityGrowth(int TotalRequired) const
        {
            if(TotalRequired <= Capacity)
                return Capacity;

            int Suggested = Capacity + Capacity / 2;
            return TotalRequired > Suggested? TotalRequired : Suggested;
        }

        T* MakeRoom(ConstIterator Pos, int Count)
        {
            if(Count <= 0)
                return const_cast<T*>(Pos);

            DifferenceType Offset = Pos - begin();
            if(Pos == end())
            {
                Reserve(Count + Size);
                Size += Count;
                return GetData() + Offset;
            }

            if(Capacity >= Size + Count)
            {
                T* InsertPos = const_cast<T*>(Pos);

                T* OldEnd = end();
                const auto AffectedNum = OldEnd - InsertPos;

                if(Count < AffectedNum)
                {
                    UninitializedTransferN<T>(OldEnd - Count, Count, OldEnd);
                    std::move_backward<Iterator>(InsertPos, OldEnd - Count, OldEnd);
                    DestroyRange(InsertPos, InsertPos + Count);
                }
                else
                {
                    UninitializedTransferN<T>(InsertPos, AffectedNum, InsertPos + Count);
                    DestroyRange(InsertPos, InsertPos + AffectedNum);
                }

                Size += Count;
                return InsertPos;
            }
            else
            {
                int NewCapacity = CalculateCapacityGrowth(Size + Count);
                T* NewHeap = static_cast<T*>(std::malloc(NewCapacity * sizeof(T)));

                UninitializedTransferN<T>(GetData(), Offset, NewHeap);
                UninitializedTransferN<T>(GetData() + Offset, Size - Offset, NewHeap + Offset + Count);

                DestroyRange(GetData(), GetData() + Size);

                SetHeap(NewHeap);
                Capacity = NewCapacity;
                Size += Count;

                return HeapData + Offset;
            }
        }

        void SetHeap(T* Heap)
        {
            if(HeapData != nullptr)
                std::free(HeapData);

            HeapData = Heap;
        }

        int Size { 0 };
        int Capacity { N };

        char Storage[N * sizeof(T)] {};
        T* HeapData { nullptr };

    };
}