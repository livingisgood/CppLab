#pragma once
#include <type_traits>
#include <iterator>

namespace BC
{
    template <typename IteratorType>
    using IteratorValueType = typename std::iterator_traits<IteratorType>::value_type;

    template<typename ForwardIt>
    void DestroyRange(ForwardIt First, ForwardIt Last) noexcept;

    template<typename T>
    void DestroyAt(T* Object)
    {
        if constexpr (std::is_array_v<T>)
        {
            DestroyRange(Object, Object + std::extent_v<T>);
        }
        else
        {
            Object->~T();
        }
    }

    template<typename ForwardIt>
    void DestroyRange(ForwardIt First, ForwardIt Last) noexcept
    {
        if constexpr (!std::is_trivially_destructible_v<IteratorValueType<ForwardIt>>)
        {
            for(; First != Last; ++First)
            {
                DestroyAt(std::addressof(*First));
            }
        }
    }
}
