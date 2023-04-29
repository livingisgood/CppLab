#pragma once
#include <cstddef>
#include <utility>

namespace BC
{
    class FMemoryPool
    {
        struct FBlock
        {
            unsigned char Head {0};
            FBlock* Next {nullptr};

            int GetBlockSizeIndex() const { return Head >> 1; }
            bool InUse() const { return (Head & 1) > 0; }

            void SetInUse(bool bInUse)
            {
                Head = ((Head >> 1) << 1) + (bInUse? 1 : 0);
            }

            void SetSize(int SizeIndex)
            {
                Head = (SizeIndex << 1) + (Head & 1);
            }
        };

    public:

        static constexpr int MaxSizePower = 11;
        static constexpr std::size_t MaxBlockSize = 1 << MaxSizePower;

        FMemoryPool();
        ~FMemoryPool();

        FMemoryPool(const FMemoryPool& Other) = delete;
        FMemoryPool& operator=(const FMemoryPool& Other) = delete;

        template<typename T, typename...ArgType>
        T* Construct(ArgType... Args)
        {
            constexpr std::size_t RequiredSize = sizeof(T) + sizeof(FBlock);
            static_assert(RequiredSize <= MaxBlockSize, "this type of object is too big.");

            void* Block = Allocate(GetSizeIndex(RequiredSize));
            new (Block) T(std::forward<ArgType>(Args)...);
            return static_cast<T*>(Block);
        }

        template<typename T>
        void Destruct(T* Object)
        {
            if(Recycle(Object))
                Object->~T();
        }

        static int GetSizeIndex(std::size_t BlockSize);

    private:

        void* Allocate(std::size_t BlockSize);

        bool Recycle(void* UserObjectPtr);

        FBlock FreeBlocks[MaxSizePower + 1] {};
    };
}
