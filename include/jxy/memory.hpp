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
#include <wdm.h>
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

    _CONSTEXPR20_DYNALLOC ~allocator() = default;
    _CONSTEXPR20_DYNALLOC allocator& operator=(const allocator&) = default;

    _CONSTEXPR20_DYNALLOC
    void deallocate(
        value_type* const Memory,
        const size_type)
    {
        if (Memory)
        {
            ExFreePoolWithTag(Memory, t_PoolTag);
        }
    }

    _NODISCARD _CONSTEXPR20_DYNALLOC
    __declspec(allocator)
    value_type* allocate(_CRT_GUARDOVERFLOW const size_type Count)
    {
        auto memory = static_cast<value_type*>(
            ExAllocatePoolWithTag(t_PoolType,
                                  (sizeof(value_type) * Count),
                                  t_PoolTag));
        if (!memory)
        {
            throw std::bad_alloc();
        }
        return memory;
    }

#if _HAS_DEPRECATED_ALLOCATOR_MEMBERS
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS typedef T* pointer;
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS typedef const T* const_pointer;

    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS typedef T& reference;
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS typedef const T& const_reference;

    using is_always_equal _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS = std::true_type;

    template <typename Other>
    struct _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS rebind 
    {
        using other = allocator<Other, t_PoolType, t_PoolTag>;
    };

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
        ::new (std::_Voidify_iter(Memory)) U(std::forward<TArgs>(Args)...);
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

//
// For shared_ptr, here, I might roll a more allocation-friendly solution
// later. shared_ptr will internally use the default `new` allocator, which
// is something I prefer to avoid, in order to force pool and tag
// specifications. Having a global allocator for kernel code is "meh". So I
// might replace this with something a bit more kernel friendly. For now, this
// is okay, and I'll hack-in my own make_shared which will help enforce use of
// being pool/tag considerate.
//

template <typename T, POOL_TYPE t_PoolType, ULONG t_PoolTag>
using shared_ptr = std::shared_ptr<T>;

template <typename T, POOL_TYPE t_PoolType, ULONG t_PoolTag, typename... TArgs>
shared_ptr<T, t_PoolType, t_PoolTag> make_shared(TArgs&&... Args) noexcept(false)
{
    return std::shared_ptr<T>(
        new(t_PoolType, t_PoolTag) T(std::forward<TArgs>(Args)...),
        jxy::default_delete<T, t_PoolType, t_PoolTag>(),
        jxy::allocator<T, t_PoolType, t_PoolTag>());
}

}
