#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <any>
#include "DataStructure/IntrusivieList.h"

namespace BC
{
    template <typename ...Args>
    class TEvent
    {
    public:

        ~TEvent()
        {
            Clear();
        }

        using FFunctor = std::function<void(Args...)>;

        class FListener
        {
            friend class TEvent;

        public:

            FListener() : Keeper(this) {}
            Intrusive::TNode<TEvent::FListener> Link {this};

            void Unsubscribe()
            {
                Link.UnLink();
                Keeper.reset();
            }

        private:

            FFunctor Function;
            std::shared_ptr<FListener> Keeper;
        };

        using FHandle = std::shared_ptr<FListener>;

        template<typename ...ArgTypes>
        void Invoke(ArgTypes&&... CallArgs) // 考虑迭代器失效...
        {
            for(auto& Listener : Listeners)
            {
                Listener.Function(std::forward<ArgTypes>(CallArgs)...);
            }
        }

        template<typename ...ArgTypes>
        void operator() (ArgTypes&&... CallArgs)
        {
            for(auto& Listener : Listeners)
            {
                Listener.Function(std::forward<ArgTypes>(CallArgs)...);
            }
        }

        FHandle Subscribe(void (*FreeFunc)(Args...))
        {
            return AddListener(FreeFunc);
        }

        template <class CallerClass, typename ...ArgTypes>
        FHandle Subscribe(CallerClass* Caller, void (CallerClass::*Member)(ArgTypes...))
        {
            return AddListener([Caller,Member](ArgTypes&&...CallArgs){ (Caller->*Member)(std::forward<ArgTypes>(CallArgs)...); });
        }

        template <class CallerClass, typename ...ArgTypes>
        FHandle Subscribe(const CallerClass* Caller, void (CallerClass::*ConstMember)(ArgTypes...) const)
        {
            return AddListener([Caller,ConstMember](ArgTypes&&...CallArgs){ (Caller->*ConstMember)(std::forward<ArgTypes>(CallArgs)...); });
        }

        void Clear()
        {
            for(auto It = Listeners.begin(); It != Listeners.end();)
            {
                FListener& Listener = *It;
                It = Listeners.Erase(It);

                Listener.Keeper.reset();
            }
        }

    private:

        FHandle AddListener(FFunctor Function)
        {
            FListener* Listener = new FListener;
            Listener->Function = std::move(Function);

            Listeners.PushBack(*Listener);

            return Listener->Keeper;
        }

        Intrusive::TNodeList<TEvent::FListener, &TEvent::FListener::Link> Listeners;
    };
}