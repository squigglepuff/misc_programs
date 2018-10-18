#ifndef CLIST_H
#define CLIST_H

/*
    Problem: Create a singly linked list API that takes a void pointer and stores it. DO NOT COPY REFERENCED DATA!
    The API should allow the client to manipulate the list indirectly (i.e. insert/remove items) and for them to
    aquire the head node for iteration.

    Solution: API will implement the following methods:
    (1) Append(void *apPtr),
    (2) Prepend(void *apPtr),
    (3) Insert(SNode *apPrevNode, void *apPtr),
    (4) RemoveFirst(),
    (5) RemoveLast(),
    (6) Remove(SNode *apPrevNode),
    (7) Head(),
    (8) End,
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
using namespace std;

class CList
{
private:

    struct SNode
    {
        void *mpPtr;
        SNode *mpPrevNodePtr;
        SNode *mpNextNodePtr;

        SNode() : mpPtr(nullptr), mpPrevNodePtr(nullptr), mpNextNodePtr(nullptr) {}
        SNode(void *apPtr, SNode* apPrevNodePtr, SNode* apNextNodePtr) : mpPtr(apPtr), mpPrevNodePtr(apPrevNodePtr), mpNextNodePtr(apNextNodePtr) {}
        SNode(const SNode& aNode) : mpPtr(aNode.mpPtr), mpPrevNodePtr(aNode.mpPrevNodePtr), mpNextNodePtr(aNode.mpNextNodePtr) {}
    };

     // These are not to be access by the client.
    CList(const CList&);
    CList& operator=(const CList&);

public:
    typedef SNode* NodePtr;

    CList();
    ~CList();

    bool Append(void *apPtr); //<! Returns true if append was successful.
    bool Prepend(void *apPtr); //<! Returns true if prepend was successfuly.
    bool Insert(NodePtr apNextNode, void *apPtr); //<! Returns true if insert was successful.

    bool RemoveFirst(); //<! Returns true if removefirst was successful.
    bool RemoveLast(); //<! Returns true if removelast was successful.
    bool Remove(NodePtr apNodeToRemove); //<! Returns true if remove was successful.

    NodePtr Head(); //<! Returns the first node in the list (the head).
    NodePtr End(); //<! Returns the last node in the list (the tail).
    void Clear(); //<! Clears the ENTIRE list.
    size_t NumItemsInList(); //<! Returns number of items the list contains [1 - n].

private:
    bool DeleteHead();

    NodePtr mpHeadNode;
    size_t miNumListItems;
    NodePtr mpEndNode;
};

#endif // CLIST_H
