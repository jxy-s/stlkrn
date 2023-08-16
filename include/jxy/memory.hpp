//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/memory.hpp
// Author:   Johnny Shaw
// Abstract: Memory allocators, deleters, and smart pointers.
//
// The allocators for jxylib are designed around pool typing and tagging.
// jxy::allocator is implemented at the MSVC STL std::memory is, however
// it requires you specify the pool type and tag. The jxy::allocator is then
// used to template-into other containers that take allocators. For example
// the jxy::vector requires you specify the pool type and tag, then uses
// the std::vector by providing the jxy::allocator to it.
//
// jxylib               STL equivalent
// ---------------------------------------------------------------------------
// jxy::allocator       std::allocator
// jxy::default_delete  std::default_delete
// jxy::unique_ptr      std::unique_ptr
// jxy::shared_ptr      std::shared_ptr
//
#pragma once
#include <fltKernel.h>
#include <jxy/alloc.hpp>
#include <memory>

namespace jxy
{

namespace details
{

template <typename T, POOL_TYPE t_PoolType, ULONG t_PoolTag>
class allocator
{
public:

    static_assert(!std::is_const_v<T>, 
                  "The C++ Standard forbids containers of const elements "
                  "because allocator<const T> is ill-formed.");

    static constexpr POOL_TYPE pool_type = t_PoolType;
    static constexpr ULONG pool_tag = t_PoolTag;

    using _From_primary = allocator;

    using value_type = T;

    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using propagate_on_container_move_assignment = std::true_type;

    constexpr allocator() noexcept {}

    constexpr allocator(const allocator&) noexcept = default;

    template <typename Other>
    constexpr allocator(const allocator<Other, t_PoolType, t_PoolTag>&) noexcept
    {
    }

    _CONSTEXPR20 ~allocator() = default;
    _CONSTEXPR20 allocator& operator=(const allocator&) = default;

    _CONSTEXPR20
    void deallocate(
        value_type* const Memory,
        const size_type)
    {
        if (Memory)
        {
            ExFreePoolWithTag(Memory, t_PoolTag);
        }
    }

    _NODISCARD _CONSTEXPR20
    __declspec(allocator)
    value_type* allocate(_CRT_GUARDOVERFLOW const size_type Count)
    {
#pragma warning(push)
#pragma warning(disable : 4996) // FIXME - deprecated function
        auto memory = static_cast<value_type*>(
            ExAllocatePoolWithTag(t_PoolType,
                                  (sizeof(value_type) * Count),
                                  t_PoolTag));
#pragma warning(pop)
        if (!memory)
        {
            throw std::bad_alloc();
        }
        return memory;
    }

    //
    // jxy::allocator implements rebind despite it being deprecated (cpp17) and
    // removed (cpp20) on std::allocator.
    // Without this, std::_Replace_first_parameter<_Other,_Ty> trips up on replacing
    // the type at rebind_alloc on allocator_traits.
    //
    // Per the allocator concepts - implementation of rebind is optional and seemingly
    // required for us to avoid the template parameter replacement on the custom
    // allocator:
    // https://en.cppreference.com/w/cpp/named_req/Allocator#cite_note-2
    //

    template <typename Other>
    struct rebind
    {
        using other = allocator<Other, t_PoolType, t_PoolTag>;
    };

#if _HAS_DEPRECATED_ALLOCATOR_MEMBERS
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS typedef T* pointer;
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS typedef const T* const_pointer;

    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS typedef T& reference;
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS typedef const T& const_reference;

    using is_always_equal _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS = std::true_type;

    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS 
    _NODISCARD 
    T* address(T& Val) const noexcept 
    {
        return std::addressof(Val);
    }

    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS 
    _NODISCARD
    const T* address(const T& Val) const noexcept 
    {
        return std::addressof(Val);
    }

    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS _NODISCARD 
    __declspec(allocator) 
    T* allocate(_CRT_GUARDOVERFLOW const size_type _Count, const void*) 
    {
        return allocate(_Count);
    }

    template <class U, class... TArgs>
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS 
    void construct(U* const Memory, TArgs&&... Args) 
    {
        ::new (const_cast<void*>(static_cast<const volatile void*>(Memory))) U(std::forward<TArgs>(Args)...);
    }

    template <class U>
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS 
    void destroy(U* const Memory) 
    {
        Memory->~U();
    }

    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS 
    _NODISCARD 
    size_type max_size() const noexcept 
    {
        return static_cast<size_type>(-1) / sizeof(T);
    }
#endif // _HAS_DEPRECATED_ALLOCATOR_MEMBERS

};

template <typename T, POOL_TYPE t_PoolType, ULONG t_PoolTag>
class default_delete
{
public:

    static constexpr POOL_TYPE pool_type = t_PoolType;
    static constexpr ULONG pool_tag = t_PoolTag;

    using value_type = T;

    constexpr default_delete() noexcept = default;

    template <typename Other, std::enable_if_t<
                              (std::is_convertible_v<T*, typename Other::value_type*> &&
                              (t_PoolType == Other::pool_type) &&
                              (t_PoolTag == Other::pool_tag)), int> = 0>
    default_delete(const default_delete<typename Other::value_type, 
                                        Other::pool_type, 
                                        Other::pool_tag>&) noexcept 
    {
    }

    void operator()(T* Pointer) const noexcept
    {
        static_assert(0 < sizeof(Pointer), "can't delete an incomplete type");
        if (Pointer)
        {
            Pointer->~T();
            ExFreePoolWithTag(Pointer, t_PoolTag);
        }
    }

};

}

template <typename T, POOL_TYPE t_PoolType, ULONG t_PoolTag>
using allocator = details::allocator<T, t_PoolType, t_PoolTag>;

template <typename T, ULONG t_PoolTag>
using paged_allocator = details::allocator<T, PagedPool, t_PoolTag>;

template <typename T, ULONG t_PoolTag>
using non_paged_allocator = details::allocator<T, NonPagedPoolNx, t_PoolTag>;

template <typename T, POOL_TYPE t_PoolType, ULONG t_PoolTag>
using default_delete = details::default_delete<T, t_PoolType, t_PoolTag>;

template <typename T, ULONG t_PoolTag>
using default_paged_delete = details::default_delete<T, PagedPool, t_PoolTag>;

template <typename T, ULONG t_PoolTag>
using default_non_paged_delete = details::default_delete<T, NonPagedPoolNx, t_PoolTag>;

template <typename T, POOL_TYPE t_PoolType, ULONG t_PoolTag, typename TDeleter = default_delete<T, t_PoolType, t_PoolTag>>
using unique_ptr = std::unique_ptr<T, TDeleter>;

template <typename T, POOL_TYPE t_PoolType, ULONG t_PoolTag, typename... TArgs>
unique_ptr<T, t_PoolType, t_PoolTag> make_unique(TArgs&&... Args) noexcept(false)
{
    return unique_ptr<T, t_PoolType, t_PoolTag>(new(t_PoolType, t_PoolTag)T(std::forward<TArgs>(Args)...));
}

template <typename T, POOL_TYPE t_PoolType, ULONG t_PoolTag>
using shared_ptr = std::shared_ptr<T>;

template <typename T, POOL_TYPE t_PoolType, ULONG t_PoolTag, typename... TArgs>
shared_ptr<T, t_PoolType, t_PoolTag> make_shared(TArgs&&... Args) noexcept(false)
{
    //
    // Rather than use make_shared here, we'll force allocate_shared.
    // This is because there is no way to specify an allocator on make_shared 
    // thus internally it will use the global new allocator, since jxystl aims
    // to force tagging and typing of all allocations we avoid that at all
    // costs.
    // 
    // allocate_shared enables us to specify jxy::allocator with our pool
    // parameters and eliminates this problem.
    //

    using allocator = jxy::allocator<T, t_PoolType, t_PoolTag>;
    return std::allocate_shared<T, allocator>(allocator(), std::forward<TArgs>(Args)...);
}

}
