#ifndef CLIST_H
#define CLIST_H

/*
    Problem: Create a singly linked list API that takes a void pointer and stores it. DO NOT COPY REFERENCED DATA!
    The API should allow the client to manipulate the list indirectly (i.e. insert/remove items) and for them to
    aquire the head node for iteration.

    Solution: API will implement the following methods:
    (1) Append(tType apPtr),
    (2) Prepend(tType apPtr),
    (3) Insert(SNode *apPrevNode, tType apPtr),
    (4) RemoveFirst(),
    (5) RemoveLast(),
    (6) Remove(SNode *apPrevNode),
    (7) Head(),
    (8) End(),
    (9) Clear(),
    (10) NumItemsInList().

    In order, these are as defined:
    (1) Place supplied pointer at end of list,
    (2) Place supplied pointer at beginning of list,
    (3) insert supplied pointer after supplied node,
    (4) remove first item,
    (5) remove last item,
    (6) remove item after supplied node,
    (7) return the Head node,
    (8) return the End node,
    (9) Removes ALL the list items,
    (10) return the number of items the list contains.

    Returning the a node with Head() or End() will allow the client to iterate through the list items as he/she sees fit.
    Which then allows the client to supply the correct arguments to Insert() and Remove(). This does, although, mean the
    list API will NOT iterate through the items; This is the responsiblity of the client. Also, since the API takes a
    void pointer, the list will NEVER contain anything other than void pointers. The API will NOT copy the referenced data
    into it's own buffer (particulary because it has no internal buffer). As such, the API will NEVER alloc or delete the
    referenced data, only delete the specified list node.
*/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <typeinfo>
using namespace std;

template<typename tType>
class CList
{
    struct SNode
    {
        tType mtData;
        SNode *mpPrevNodePtr;
        SNode *mpNextNodePtr;

        SNode() : mpPrevNodePtr(nullptr), mpNextNodePtr(nullptr) {}
        SNode(tType const &atData, SNode* apPrevNodePtr, SNode* apNextNodePtr) : mtData(atData), mpPrevNodePtr(apPrevNodePtr), mpNextNodePtr(apNextNodePtr) {}
        SNode(const SNode& aNode) : mtData(aNode.mtData), mpPrevNodePtr(aNode.mpPrevNodePtr), mpNextNodePtr(aNode.mpNextNodePtr) {}
        SNode& operator=(const SNode& aNode) { mtData = aNode.mtData; mpPrevNodePtr = aNode.mpPrevNodePtr; mpNextNodePtr = aNode.mpNextNodePtr; return *this; }
    };

    size_t miNumListItems;
    SNode* mpHeadNode;
    SNode* mpEndNode;

    bool DeleteHead()
    {
        if (nullptr == mpHeadNode)
        {
            return false;
        }

        if (mpHeadNode != mpEndNode)
        {
            return false;
        }

        assert(1 == NumItemsInList());
        miNumListItems = 0;

        SNode *pTmpHead = mpHeadNode;

        mpHeadNode = nullptr;
        mpEndNode = nullptr;

        delete pTmpHead;

        return true;
    }

public:
    typedef SNode* NodePtr;

    CList() :
        miNumListItems(0),
        mpHeadNode(nullptr),
        mpEndNode(nullptr)
    {
        // Do nothing.
    }

    CList(const CList& aCls) :
        miNumListItems(0),
        mpHeadNode(nullptr),
        mpEndNode(nullptr)
    {
        for(NodePtr pIter = aCls.Head(); nullptr != pIter; pIter = pIter->mpNextNodePtr)
        {
            Append(pIter->mtData);
        }
    }

    CList& operator=(const CList& aCls)
    {
        if (*this == aCls)
        {
            return *this;
        }

        miNumListItems = 0;
        mpHeadNode = nullptr;
        mpEndNode = nullptr;

        for(NodePtr pIter = aCls.Head(); nullptr != pIter; pIter = pIter->mpNextNodePtr)
        {
            Append(pIter->mtData);
        }

        return *this;
    }

    ~CList()
    {
        // Free list memory.
        Clear();
    }

    bool Append(tType const &atData)
    {
        // Create new node for list.
        NodePtr pTmpNewNode = new SNode(atData, nullptr, nullptr);

        if (nullptr == mpHeadNode)
        {
            // Set the head.
            mpHeadNode = pTmpNewNode;
        }
        else
        {
            // Update list object pointers.
            mpEndNode->mpNextNodePtr = pTmpNewNode;
            pTmpNewNode->mpPrevNodePtr = mpEndNode;
        }

        mpEndNode = pTmpNewNode;

        ++miNumListItems;

        return (nullptr != mpEndNode);
    }

    bool Prepend(tType const &atData)
    {
        // Alloc new node to succeed the head.
        NodePtr pTmpNewNode = Head();

        // Update head node.
        mpHeadNode = new SNode(atData, nullptr, pTmpNewNode);
        mpHeadNode->mtData = atData;
        mpHeadNode->mpNextNodePtr = pTmpNewNode;
        pTmpNewNode->mpPrevNodePtr = mpHeadNode;

        ++miNumListItems;

        return true;
    }

    bool Insert(NodePtr apNextNode, tType const &atData)
    {
        if (nullptr == apNextNode)
        {
            return false;
        }

        bool bRtnValue = false;

        if (apNextNode == mpHeadNode)
        {
            bRtnValue = Prepend(atData);
        }
        else
        {
            // Get the node previous to apNextNode.
            NodePtr pPrevNode = apNextNode->mpPrevNodePtr;

            // The node was found.
            if (nullptr != pPrevNode)
            {
                // Alloc new node to be inserted.
                NodePtr pNewListNode = new SNode(atData, pPrevNode, apNextNode);

                // Update the mpNextNodePtr pointers to reflect insertion.
                pPrevNode->mpNextNodePtr = pNewListNode;
                apNextNode->mpPrevNodePtr = pNewListNode;

                ++miNumListItems;

                bRtnValue = true;
            }
        }

        return bRtnValue;
    }

    bool RemoveFirst()
    {
        if (0 == NumItemsInList() || nullptr == mpHeadNode)
        {
            return false;
        }

        if (nullptr != mpHeadNode->mpNextNodePtr)
        {
            // Since the next node after head is valid, aquire the head node, update the head node position, delete the old head.
            NodePtr pOldHeadNode = Head();

            mpHeadNode = mpHeadNode->mpNextNodePtr;
            mpHeadNode->mpPrevNodePtr = nullptr;

            --miNumListItems;

            if (1 >= miNumListItems)
            {
                mpEndNode = mpHeadNode;
            }

            delete pOldHeadNode;
        }
        else
        {
            // Remove both head and end nodes.
            return DeleteHead();
        }

        return true;
    }

    bool RemoveLast()
    {
        if (0 == NumItemsInList() || nullptr == mpHeadNode)
        {
            return false;
        }

        if (mpHeadNode != mpEndNode)
        {
            // There are more than 1 items in the list, so keep head intact.
            --miNumListItems;

            // We need to get the last list node - 1.
            NodePtr pLastListNode = End();
            NodePtr pPrevNode = End()->mpPrevNodePtr;

            // Update end node ptr.
            mpEndNode = pPrevNode;
            mpEndNode->mpNextNodePtr = nullptr;

            delete pLastListNode;
        }
        else
        {
            // Remove both head and end nodes.
            return DeleteHead();
        }

        return true;
    }

    bool Remove(NodePtr apNodeToRemove)
    {
        if (0 == NumItemsInList())
        {
            return false;
        }
        else if (nullptr == apNodeToRemove)
        {
            return false;
        }

        bool bRtnValue = false;

        if (apNodeToRemove == mpHeadNode)
        {
            bRtnValue = RemoveFirst();
        }
        else if (apNodeToRemove == mpEndNode)
        {
            bRtnValue = RemoveLast();
        }
        else if (nullptr == apNodeToRemove->mpNextNodePtr)
        {
            // We need to test for corruption in the list. The previous condition should've triggered if this condition is true.
            // Since it didn't, we need to return false. Telling the client something is wrong.
            bRtnValue = false;
        }
        else
        {
            NodePtr pPrevNode = apNodeToRemove->mpPrevNodePtr;

            if (nullptr == pPrevNode)
            {
                return false;
            }

            pPrevNode->mpNextNodePtr = apNodeToRemove->mpNextNodePtr;
            apNodeToRemove->mpNextNodePtr->mpPrevNodePtr = pPrevNode;

            --miNumListItems;

            delete apNodeToRemove;
            apNodeToRemove = nullptr;

            bRtnValue = true;
        }

        return bRtnValue;
    }

    NodePtr Head()
    {
        return mpHeadNode;
    }

    NodePtr End()
    {
        return mpEndNode;
    }

    void Clear()
    {
        // Remove list items, body was left empty intentionally.
        while (RemoveFirst()) {}

        miNumListItems = 0;
    }

    size_t NumItemsInList()
    {
        return miNumListItems;
    }
};

#endif // CLIST_H
