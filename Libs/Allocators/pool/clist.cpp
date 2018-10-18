#include "clist.h"

CList::CList()  :
    mpHeadNode(nullptr),
    miNumListItems(0),
    mpEndNode(nullptr)
{
    // Do nothing.
}

CList::~CList()
{
    // Free list memory.
    Clear();
}

bool CList::Append(void *apPtr)
{
    // Create new node for list.
    NodePtr pTmpNewNode = new SNode(apPtr, nullptr, nullptr);

    if (nullptr == mpHeadNode)
    {
        mpHeadNode = pTmpNewNode;
    }
    else
    {
        mpEndNode->mpNextNodePtr = pTmpNewNode;
        pTmpNewNode->mpPrevNodePtr = mpEndNode;
    }

    mpEndNode = pTmpNewNode;    

    ++miNumListItems;    

    return (nullptr != mpEndNode);
}

bool CList::Prepend(void *apPtr)
{
    // Alloc new node to succeed the head.
    NodePtr pTmpNewNode = Head();

    // Update head node.
    mpHeadNode = new SNode(apPtr, nullptr, pTmpNewNode);
    mpHeadNode->mpPtr = apPtr;
    mpHeadNode->mpNextNodePtr = pTmpNewNode;
    pTmpNewNode->mpPrevNodePtr = mpHeadNode;

    ++miNumListItems;

    return true;
}

bool CList::Insert(NodePtr apNextNode, void *apPtr)
{
    if (nullptr == apNextNode)
    {
        return false;
    }

    bool bRtnValue = false;

    if (apNextNode == mpHeadNode)
    {
        bRtnValue = Prepend(apPtr);
    }
    else
    {
        // Get the node previous to apNextNode.
        NodePtr pPrevNode = apNextNode->mpPrevNodePtr;

        // The node was found.
        if (nullptr != pPrevNode)
        {
            // Alloc new node to be inserted.
            NodePtr pNewListNode = new SNode(apPtr, pPrevNode, apNextNode);

            // Update the mpNextNodePtr pointers to reflect insertion.
            pPrevNode->mpNextNodePtr = pNewListNode;
            apNextNode->mpPrevNodePtr = pNewListNode;

            ++miNumListItems;

            bRtnValue = true;
        }
    }

    return bRtnValue;
}

bool CList::RemoveFirst()
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

bool CList::RemoveLast()
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

bool CList::Remove(NodePtr apNodeToRemove)
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

CList::NodePtr CList::Head()
{
    return mpHeadNode;
}

CList::NodePtr CList::End()
{
    return mpEndNode;
}

void CList::Clear()
{
    // Remove list items, body was left empty intentionally.
    while (RemoveFirst()) {}

    miNumListItems = 0;
}

size_t CList::NumItemsInList()
{
    return miNumListItems;
}

bool CList::DeleteHead()
{
    if (nullptr == mpHeadNode)
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
