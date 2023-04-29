#include "MemoryPool.h"
#include <cstdlib>
#include <New>

namespace BC
{
    FMemoryPool::FMemoryPool()
    {
        for(auto& FreeBlock : FreeBlocks)
        {
            FreeBlock = FBlock();
        }
    }

    FMemoryPool::~FMemoryPool()
    {
        for(auto& FreeBlock : FreeBlocks)
        {
            FBlock* Head = FreeBlock.Next;
            while(Head)
            {
                FBlock* MemoryBlock = Head;
                Head = Head->Next;
                std::free(MemoryBlock);
            }
        }
    }

    int FMemoryPool::GetSizeIndex(std::size_t BlockSize)
    {
        if(BlockSize > MaxBlockSize)
            return -1;

        int Bits = sizeof(std::size_t) * 4;
        int Sum = 0;
        std::size_t Origin = BlockSize;

        while(Bits > 0)
        {
            std::size_t Temp = BlockSize >> Bits;
            if(Temp > 0)
            {
                Sum += Bits;
                BlockSize = Temp;
            }

            Bits = Bits >> 1;
        }

        if((static_cast<std::size_t>(1) << Sum) < Origin)
            return Sum + 1;

        return Sum;
    }

    void* FMemoryPool::Allocate(std::size_t SizeIndex)
    {
        FBlock& Head = FreeBlocks[SizeIndex];

        FBlock* Block = Head.Next;
        if(Block != nullptr)
        {
            Block->SetInUse(true);
            Head.Next = Block->Next;

            return Block + 1;
        }
        else
        {
            std::size_t BlockSize = static_cast<std::size_t>(1) << SizeIndex;
            void* Memory = std::malloc(BlockSize);

            FBlock* InsertBlock = new (Memory) FBlock();
            InsertBlock->SetInUse(true);
            InsertBlock->SetSize(SizeIndex);

            return InsertBlock + 1;
        }
    }

    bool FMemoryPool::Recycle(void *UserObjectPtr)
    {
        FBlock* Block = (static_cast<FBlock*>(UserObjectPtr) - 1);
        if(!Block->InUse())
            return false;

        Block->SetInUse(false);
        int Index = Block->GetBlockSizeIndex();

        FBlock& Head = FreeBlocks[Index];

        Block->Next = Head.Next;
        Head.Next = Block;

        return true;
    }

}