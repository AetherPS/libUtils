#pragma once
#include "Resolver.h"

template <typename T>
class symbol
{
public:
    size_t offset;

    T* get() const
    {
        return reinterpret_cast<T*>(ResolveAddress(offset));
    }

    operator T* () const
    {
        return this->get();
    }

    T* operator->() const
    {
        return this->get();
    }
};