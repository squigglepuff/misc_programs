#ifndef CPOOL_H
#define CPOOL_H

#include "handle.h"
#include "clist.h"

template<typename tType>
class CPoolAllocator
{
    CPoolAllocator(const CPoolAllocator&) {}
    CPoolAllocator& operator=(const CPoolAllocator&)
    {
        return *this;
    }

public:
    typedef CHandle<tType> CMemHandle;

    CPoolAllocator(u32 aNumObjects = 128)
    {
        for (u32 iCounter = 0; iCounter < aNumObjects; ++iCounter)
        {
            CMemHandle *mpHandle = new CMemHandle();

            mFree.Append(mpHandle);
        }

        miSize = aNumObjects;
        miNumObjs = 0;
    }

    ~CPoolAllocator()
    {
        for (CList::NodePtr pIter = mFree.Head(); pIter != mFree.End(); ++pIter)
        {
            CMemHandle* pTmp = static_cast<CMemHandle*>(pIter->mpPtr);
            delete pTmp;
        }

        for (CList::NodePtr pIter = mUsed.Head(); pIter != mUsed.End(); ++pIter)
        {
            CMemHandle* pTmp = static_cast<CMemHandle*>(pIter->mpPtr);
            delete pTmp;
        }

        mFree.Clear();
        mUsed.Clear();
    }

    CMemHandle* GetObject()
    {
        if (miNumObjs < miSize)
        {
            for (CList::NodePtr pIter = mFree.Head(); pIter != mFree.End(); ++pIter)
            {
                ++miNumObjs;
                CMemHandle *pRtn = static_cast<CMemHandle*>(pIter->mpPtr);
                mFree.Remove(pIter);
                mUsed.Append(pRtn);
                return pRtn;
            }
        }

        return nullptr;
    }

    void FreeObject(CMemHandle *aHandle)
    {
        if (0 < miNumObjs)
        {
            for (CList::NodePtr pIter = mUsed.Head(); pIter != mUsed.End(); ++pIter)
            {
                if (aHandle == pIter->mpPtr)
                {
                    --miNumObjs;
                    CMemHandle *pTmp = static_cast<CMemHandle*>(pIter->mpPtr);
                    mUsed.Remove(pIter);
                    mFree.Append(pTmp);
                }
            }
        }
    }

private:
    CList mFree;
    CList mUsed;
    u32 miSize;
    u32 miNumObjs;
};

#endif // CPOOL_H
