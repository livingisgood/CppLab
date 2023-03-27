#pragma once
#include <type_traits>
#include <iterator>

namespace BC
{
    template <typename IteratorType>
    using IteratorValueType = typename std::iterator_traits<IteratorType>::value_type;

    template<typename ForwardIt, typename std::enable_if_t<std::is_trivially_destructible<IteratorValueType<ForwardIt>>::value>*>
    void DestroyRange(ForwardIt First, ForwardIt Last) noexcept;

    template<typename ForwardIt, typename std::enable_if_t<!std::is_trivially_destructible<IteratorValueType<ForwardIt>>::value>*>
    void DestroyRange(ForwardIt First, ForwardIt Last) noexcept;

    template<typename T, typename std::enable_if_t<std::is_array<T>::value>* = nullptr>
    void DestroyObject(T& Object) noexcept
    {
        DestroyRange(Object, Object + std::extent_v<T>);
    }

    template<typename T, typename std::enable_if_t<!std::is_array<T>::value>* = nullptr>
    void DestroyObject(T& Object) noexcept
    {
        Object.~T();
    }

    template<typename ForwardIt, typename std::enable_if_t<std::is_trivially_destructible<IteratorValueType<ForwardIt>>::value>* = nullptr>
    void DestroyRange(ForwardIt First, ForwardIt Last) noexcept
    {
        // do nothing
    }

    template<typename ForwardIt, typename std::enable_if_t<!std::is_trivially_destructible<IteratorValueType<ForwardIt>>::value>* = nullptr>
    void DestroyRange(ForwardIt First, ForwardIt Last) noexcept
    {
        for(; First != Last; ++First)
        {
            DestroyObject(*First);
        }
    }

    template<typename T, typename SizeType, typename std::enable_if_t<std::is_trivially_copyable<T>::value>* = nullptr>
    void UninitializedTransferN(T* First, SizeType Count, T* Dest)
    {
        if(Count > 0)
            std::memmove(Dest, First, Count * sizeof(T));
    }

    template<typename T, typename SizeType, typename std::enable_if_t<!std::is_trivially_copyable<T>::value>* = nullptr>
    void UninitializedTransferN(T* First, SizeType Count, T* Dest)
    {
        if(Count <= 0)
            return;

        for(SizeType i = 0; i < Count; ++i)
        {
            new (static_cast<void*>(Dest + i)) T(std::move(*(First + i)));
        }
    }

}
