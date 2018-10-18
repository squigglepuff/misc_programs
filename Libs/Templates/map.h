/*
 ***************************************
 * Simple map container library
 * ©2014 Travis Ervin
 * 4 Spaces Per TAB
 * 120 Columns Per Page
 ***************************************
 *
 * API Specification *
 * Add(key, item)               -   Adds the given item to the end of the map with the given key.
 * Remove(key)                  -   Removes the given key and it's item from the map.
 * GetItem(key)                 -   Returns the item at the given key.
 * GetKey(item)                 -   Returns the key for the given item.
 * ReplaceItem(key, item)       -   Replaces the item at the key with the given item.
 * ReplaceKey(oldKey, newKey)   -   Replaces the oldKey with the newKey.
 * Clear()                      -   Clears the entire map of all keys and items.
 *
 * operator=(object)            -   Does a deep copy from object to self.
 * operator[](index)            -   Returns the item at the given index.
 */

#ifndef MAP_H
#define MAP_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

template<typename tKey, typename tType>
class CMap
{
public:
    CMap() {}
    CMap(const CMap& aCls): mVector(aCls.mVector) {}
    ~CMap()
    {
        if (!mVector.empty())
        {
            mVector.clear();
        }
    }

    CMap& operator=(const CMap& aCls)
    {
        if (this != &aCls)
        {
            std::copy(aCls.mVector.begin(), aCls.mVector.end(), mVector);
        }

        return *this;
    }

    tType& operator[](const size_t aIdx)
    {
        return mVector.at(aIdx).second;
    }

    // These all return true upon success and false upon failure.
    bool Add(tKey aKey, tType aItem)
    {
        size_t iOldSize = mVector.size();
        mVector.push_back(std::pair<tKey, tType>(aKey, aItem));

        if (iOldSize < mVector.size())
        {
            return true;
        }

        return false;
    }

    bool Remove(tKey aKey)
    {
        size_t iIdx = 0;
        for (iIdx = 0; iIdx < mVector.size(); ++iIdx)
        {
            if (mVector.at(iIdx).first == aKey)
            {
                break;
            }
        }

        if (iIdx < mVector.size())
        {
            //remove the given element.
            const size_t iPos = static_cast<const size_t>(iIdx);
            mVector.erase(mVector.begin() + iPos);
            return true;
        }

        return false;
    }

    bool ReplaceItem(tKey aKey, tType aItem)
    {
        size_t iIdx = 0;
        for (iIdx = 0; iIdx < mVector.size(); ++iIdx)
        {
            if (mVector.at(iIdx).first == aKey)
            {
                break;
            }
        }

        if (iIdx < mVector.size())
        {
            mVector.at(iIdx).second = aItem;
            return true;
        }

        return false;
    }

    bool ReplaceKey(tKey aOldKey, tKey aNewKey)
    {
        size_t iIdx = 0;
        for (iIdx = 0; iIdx < mVector.size(); ++iIdx)
        {
            if (mVector.at(iIdx).first == aOldKey)
            {
                break;
            }
        }

        if (iIdx < mVector.size())
        {
            mVector.at(iIdx).first = aNewKey;
            return true;
        }

        return false;
    }

    bool SwapKeys(tType aFirst, tType aSecond)
    {
        tKey tFirst = GetKey(aFirst);
        tKey tSecond = GetKey(aSecond);

        int iFirstIdx = -1;
        int iSecondIdx = -1;
        for (size_t iIdx = 0; iIdx < mVector.size(); ++iIdx)
        {
            if (mVector.at(iIdx).first == tFirst)
            {
                iFirstIdx = iIdx;
            }
            else if (mVector.at(iIdx).first == tSecond)
            {
                iSecondIdx = iIdx;
            }

            if (0 <= iFirstIdx && 0 <= iSecondIdx)
            {
                break;
            }
        }

        if (0 <= iFirstIdx && 0 <= iSecondIdx)
        {
            mVector.at(iFirstIdx).first = tSecond;
            mVector.at(iSecondIdx).first = tFirst;
            return true;
        }

        return false;
    }

    bool Clear()
    {
        mVector.clear();

        return mVector.size() == 0 ? true : false;
    }

    // Retrieval functions.
    tType GetItem(tKey aKey)
    {
        size_t iIdx = 0;
        for (iIdx = 0; iIdx < mVector.size(); ++iIdx)
        {
            if (mVector.at(iIdx).first == aKey)
            {
                return mVector.at(iIdx).second;
            }
        }

        return NULL;
    }

    tKey GetKey(tType aItem)
    {
        size_t iIdx = 0;
        for (iIdx = 0; iIdx < mVector.size(); ++iIdx)
        {
            if (mVector.at(iIdx).second == aItem)
            {
                return mVector.at(iIdx).first;
            }
        }

        return NULL;
    }

    size_t GetSize()
    {
        return mVector.size();
    }

private:
    std::vector< std::pair<tKey, tType> > mVector;
};

#endif // MAP_H
