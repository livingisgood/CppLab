#pragma once
#include <cstddef>

namespace BC
{
    class FAllocator
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




    private:

        int MaxSizePower;

    };
}