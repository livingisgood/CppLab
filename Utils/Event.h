#pragma once
#include <functional>
#include <memory>
#include "DataStructure/IntrusivieList.h"

namespace BC
{
    class FListenerBase
    {
        template <typename ...Args> friend class TEvent;

    public:

        virtual ~FListenerBase() = default;

        void UnSubscribe() { bValid = false; }

    private:

        bool bValid {true};
        std::shared_ptr<FListenerBase> Keeper {this};
    };

    using FEventHandle = std::shared_ptr<FListenerBase>;

    class FListenerKeeper
    {
    public:

        ~FListenerKeeper()
        {
            for(auto& Listener : Listeners)
            {
                if(Listener)
                    Listener->UnSubscribe();
            }
        }

        void Keep(const std::shared_ptr<FListenerBase>& Listener)
        {
            Listeners.emplace_back(Listener);
        }

    private:
        std::vector<std::shared_ptr<FListenerBase>> Listeners;
    };

    template<typename ...Args>
    class TEvent
    {
        using FFunctor = std::function<void(Args...)>;

        class FListener : public FListenerBase
        {
            friend class TEvent;

        public:

            Intrusive::TNode<FListener> Link {this};
            FFunctor CallBack;
        };

    public:

        ~TEvent() { Clear(); }

        template<typename ...ArgTypes>
        void Invoke(ArgTypes&&... CallArgs)
        {
            for(FListener& Listener : ListenerList)
            {
                if(Listener.bValid)
                {
                    Listener.CallBack(std::forward<ArgTypes>(CallArgs)...);
                }
                else
                {
                    Listener.Link.UnLink();
                    Listener.Keeper.reset();
                }
            }
        }

        template<typename ...ArgTypes>
        void operator() (ArgTypes&&... CallArgs)
        {
            Invoke(std::forward<ArgTypes>(CallArgs)...);
        }

        FEventHandle Subscribe(void (*FreeFunc)(Args...))
        {
            return AddListener(FreeFunc);
        }

        template <typename CallerClass, typename ...ArgTypes>
        [[nodiscard]] FEventHandle Subscribe(CallerClass* Caller, void (CallerClass::*Member)(ArgTypes...))
        {
            return AddListener([Caller,Member](ArgTypes&&...CallArgs){ (Caller->*Member)(std::forward<ArgTypes>(CallArgs)...); });
        }

        template <typename CallerClass, typename ...ArgTypes>
        [[nodiscard]] FEventHandle Subscribe(const CallerClass* Caller, void (CallerClass::*ConstMember)(ArgTypes...) const)
        {
            return AddListener([Caller,ConstMember](ArgTypes&&...CallArgs){ (Caller->*ConstMember)(std::forward<ArgTypes>(CallArgs)...); });
        }

        void Clear()
        {
            for(auto It = ListenerList.begin(); It != ListenerList.end(); ++It)
            {
                FListener& Listener = *It;
                ListenerList.Erase(It);

                Listener.Keeper.reset();
            }
        }

    private:

        FEventHandle AddListener(FFunctor Function)
        {
            auto Listener = new FListener;
            Listener->CallBack = std::move(Function);

            ListenerList.PushBack(*Listener);
            return std::static_pointer_cast<FListenerBase>(Listener->Keeper);
        }

        Intrusive::TNodeList<FListener, &FListener::Link> ListenerList;
    };
}
