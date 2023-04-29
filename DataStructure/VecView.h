#pragma once
#include <vector>
#include "BattleCore/BattleCoreUsage.h"

namespace BC
{
    template<typename T>
    class TVecView
    {
    public:

        TVecView(T* InData, int InNum) : Data(InData), Num(InNum)
        {
            BATTLE_CHECK(Num >= 0);
        }

        explicit TVecView(const std::vector<T>& Vec) : Data(Vec.data()), Num(static_cast<int>(Vec.size()))
        {
            BATTLE_CHECK(Num >= 0);
        };

        T* GetData() const { return Data; }
        int GetNum() const { return Num; }

        T* begin() const { return Data; }
        T* end() const { return Data + Num; }

    private:

        T* Data {nullptr};
        int Num {0};
    };
}