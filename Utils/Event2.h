#pragma once
#include <functional>
#include <any>
#include <memory>
#include "DataStructure/IntrusivieList.h"

namespace BC
{
    class FListener
    {
        template<typename ...Args> friend class TEvent;
        friend class FEventHandle;

    public:

        Intrusive::TNode<FListener> Link {this};

    private:

        std::any Callback;
        std::shared_ptr<FListener> Keeper;
    };

    class FEventHandle
    {
    public:

        ~FEventHandle()
        {
            if(Listener)
            {
                Listener->Link.UnLink();
                Listener.reset();
            }
        }



    private:

        std::shared_ptr<FListener> Listener;
    };
}
