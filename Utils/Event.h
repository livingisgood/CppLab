#pragma once

#include <functional>
#include <vector>

namespace BC
{
    struct FListenerHandle
    {
        FListenerHandle():HandleValue(0) {}

        bool IsValid() const { return HandleValue != 0; }

        void Invalidate() { HandleValue = 0; }

        bool operator==(const FListenerHandle& Other) const { return HandleValue == Other.HandleValue; }

        bool operator!=(const FListenerHandle& Other) const { return HandleValue != Other.HandleValue; }

        inline uint32_t GetIndex() const { return static_cast<uint32_t>(HandleValue & IndexMask);}

        static FListenerHandle Generate(uint32_t Index)
        {
            ++IDSeed;
            if(IDSeed >= IDLimit)
            {
                IDSeed = 1;
            }
            uint64_t NewID = IDSeed;

            FListenerHandle Handle;
            Handle.SetValue(Index, NewID);
            return Handle;
        }

    private:

        void SetValue(uint32_t Index, uint64_t ID)
        {
            HandleValue = (ID << IndexBits) | static_cast<uint64_t>(Index);
        }

        static std::uint64_t IDSeed;

        static constexpr  uint32_t IndexBits = 24;
        static constexpr  uint32_t UniqueIDBits = 64 - IndexBits;
        static constexpr  uint64_t IndexMask = static_cast<uint64_t>(1 << IndexBits) - 1;
        static constexpr  uint64_t IDLimit = static_cast<uint64_t>(1) << UniqueIDBits;

        uint64_t HandleValue;
    };

    template <class ...Args>
    class TEvent
    {

        using FFunctor = std::function<void(Args...)>;
        struct FListener
        {
            FListenerHandle Handle;
            FFunctor Function;
            const void* Caller {nullptr}; // could be null if function is not a member function

            FListener(FListenerHandle Handle, FFunctor Func) : Handle(Handle), Function(std::move(Func)) {}
            FListener(FListenerHandle Handle, FFunctor Func, const void* InCaller) : Handle(Handle), Function(std::move(Func)), Caller(InCaller) {}

            void Invalidate()
            {
                Handle.Invalidate();
                Caller = nullptr;
            }

            bool IsValid() const
            {
                return Handle.IsValid();
            }

            bool IsValidMatch(const FListenerHandle& InHandle) const
            {
                return IsValid() && InHandle == Handle;
            }
        };

    public:

        template<typename ...ArgTypes>
        void Invoke(ArgTypes&&... CallArgs)
        {
            for (auto &Listener : ListenerList)
            {
                if (Listener.Handle.IsValid())
                {
                    Listener.Function(std::forward<ArgTypes>(CallArgs)...);
                }
            }
        }

        template<typename ...ArgTypes>
        void operator() (ArgTypes&&...CallArgs)
        {
            for (auto &Listener : ListenerList)
            {
                if (Listener.Handle.IsValid())
                {
                    Listener.Function(std::forward<ArgTypes>(CallArgs)...);
                }
            }
        }

        FListenerHandle Subscribe(void (*FreeFunc)(Args...))
        {
            return AddListener(FreeFunc);
        }

        template <class CallerClass, typename ...ArgTypes>
        FListenerHandle Subscribe(CallerClass* Caller, void (CallerClass::*Member)(ArgTypes...))
        {
            return AddListener([Caller,Member](ArgTypes&&...CallArgs){ (Caller->*Member)(std::forward<ArgTypes>(CallArgs)...); }, Caller);
        }

        template <class CallerClass, typename ...ArgTypes>
        FListenerHandle Subscribe(const CallerClass* Caller, void (CallerClass::*ConstMember)(ArgTypes...) const)
        {
            return AddListener([Caller,ConstMember](ArgTypes&&...CallArgs){ (Caller->*ConstMember)(std::forward<ArgTypes>(CallArgs)...); }, Caller);
        }

        void UnSubscribe(FListenerHandle Handle)
        {
            uint32_t Index = Handle.GetIndex();
            if(Index < ListenerList.size())
            {
                FListener& Listener = ListenerList[Index];

                if(Listener.IsValidMatch(Handle))
                {
                    Listener.Invalidate();
                    FreeList.push_back(Index);
                }
            }
        }

        void UnSubscribe(const void* Caller)
        {
            if(Caller == nullptr)
                return;

            for(uint32_t i = 0; i < ListenerList.size(); ++i)
            {
                FListener& Listener = ListenerList[i];

                if(Listener.Caller == Caller && Listener.IsValid())
                {
                    Listener.Invalidate();
                    FreeList.push_back(i);
                }
            }
        }

        void ClearAllListener()
        {
            ListenerList.clear();
            FreeList.clear();
        }

    private:

        FListenerHandle AddListener(FFunctor Function, const void* Caller = nullptr)
        {
            if(FreeList.empty())
            {
                uint32_t Index = ListenerList.size();
                FListenerHandle Handle = FListenerHandle::Generate(Index);

                ListenerList.emplace_back(Handle, std::move(Function), Caller);
                return Handle;
            }
            else
            {
                uint32_t Slot = FreeList.back();
                FreeList.pop_back();

                FListenerHandle Handle = FListenerHandle::Generate(Slot);

                ListenerList[Slot].Handle = Handle;
                ListenerList[Slot].Function = std::move(Function);
                ListenerList[Slot].Caller = Caller;

                return Handle;
            }
        }

        std::vector<FListener> ListenerList;
        std::vector<uint32_t> FreeList;
    };
}