#ifndef HANDLE_H
#define HANDLE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>

#if !defined(_CUSTOM_TYPES_)
typedef unsigned char byte;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;
typedef signed char sbyte;
typedef signed short s16;
typedef signed int s32;
typedef signed long s64;

typedef void* void_ptr;
#endif

template<typename tType>
class CHandle
{    
public:
    CHandle() : mpHandle(new SHandle()) {}
    CHandle(const CHandle& aHandle) : mpHandle(aHandle.mpHandle) {}
    ~CHandle()
    {
        if (nullptr != mpHandle)
        {
            delete mpHandle;
        }
    }

    CHandle& operator=(const CHandle& aHandle)
    {
        if (this != &aHandle)
        {
            if (nullptr == mpHandle)
            {
                mpHandle = new SHandle();
            }

            *mpHandle = aHandle.*mpHandle;
        }

        return *this;
    }

    CHandle& operator=(const tType& aType)
    {
        if (nullptr == mpHandle)
        {
            mpHandle = new SHandle();
        }

        *mpHandle = aType;

        return *this;
    }

    tType& operator>>(tType& aDest)
    {
        if (nullptr != mpHandle)
        {
            aDest = mpHandle->d_mData;
        }

        return aDest;
    }

    CHandle& operator<<(tType aSource)
    {
        if (nullptr != mpHandle)
        {
            mpHandle->d_mData = aSource;
        }

        return *this;
    }

    bool operator==(const CHandle& aObj)
    {
        return (*aObj == mpHandle->d_mData);
    }

    tType& operator*()
    {
        return mpHandle->d_mData;
    }

private:
    struct SHandle
    {
        tType d_mData;
    };

    SHandle* mpHandle;
};

#endif // HANDLE_H
