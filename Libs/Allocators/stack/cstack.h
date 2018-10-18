#ifndef CSTACK_H
#define CSTACK_H

#include <cstdio>
#include <cstdlib>
#include <cstring>

#if !defined(_MAX_STACK_MEMORY_)
#define MAX_STACK 4096
#endif

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
class CStackAllocator
{
public:
    CStackAllocator() : pMax(nullptr), pTop(nullptr), pBottom(nullptr), miSize(0), pMemory(nullptr)
    {
        if (nullptr == pMemory)
        {
            pMemory = new tType[MAX_STACK];
            pBottom = pMemory;
            pTop = pBottom;
            pMax = pBottom + MAX_STACK;
        }
    }
    CStackAllocator(const CStackAllocator& aCls) : pMax(aCls.pMax), pTop(aCls.pTop), pBottom(aCls.pBottom), miSize(aCls.miSize), pMemory(nullptr)
    {
        if (nullptr == pMemory)
        {
            pMemory = new tType[MAX_STACK];
        }

        memcpy(pMemory, aCls.pMemory, MAX_STACK);
    }
    ~CStackAllocator()
    {
        if (nullptr != pMemory)
        {
            delete[] pMemory;
        }
    }

    CStackAllocator& operator=(const CStackAllocator& aCls)
    {
        if (&aCls != this)
        {
            if (nullptr == pMemory)
            {
                pMemory = new tType[MAX_STACK];
            }

            memcpy(pMemory, aCls.pMemory, MAX_STACK);
        }

        return *this;
    }

    void_ptr Allocate(u64 aAmount)
    {
        void_ptr pNewPtr = nullptr;

        if (0 < aAmount)
        {
            // We move the top_of_stack pointer up X bytes.
            u64 iAllocAmount = sizeof(tType) + aAmount;

            if (reinterpret_cast<u64>(pTop + iAllocAmount) < reinterpret_cast<u64>(pMax))
            {
                pNewPtr = pTop;
                pTop += iAllocAmount;
                miSize += iAllocAmount;
            }
        }

        return pNewPtr;
    }

    void_ptr Free(void_ptr apLastPosition)
    {
        void_ptr pNewPtr = nullptr;
        if (nullptr != apLastPosition)
        {
            long iByteAmount = reinterpret_cast<u64>(pTop) - reinterpret_cast<u64>(apLastPosition);

            if (0 < iByteAmount)
            {
                pTop = apLastPosition;
                pNewPtr = pTop;
                miSize -= iByteAmount;
            }
        }

        return pNewPtr;
    }

    void_ptr CurrentTop()
    {
        if (nullptr != pMemory)
        {
            return pTop;
        }

        return nullptr;
    }

    u64 MaxSize()
    {
        // Return the maximum stack size in bytes.
        return MAX_STACK;
    }

    u64 CurrentSize()
    {
        // Return the current stack size in bytes.
        return miSize;
    }

    bool Clear()
    {
        if (nullptr != pMemory)
        {
            Free(pBottom);
            return true;
        }

        return false;
    }

private:
    void_ptr pMax;
    void_ptr pTop;
    void_ptr pBottom;
    u64 miSize;

    tType *pMemory;
};

#endif // CSTACK_H
