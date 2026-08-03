#pragma once

#include "Impl/BinaryTreeNode.h"
#include "Memory.h"

namespace Core
{
    //for policy(unique or multiple) use separate functions
    template<typename K, typename V, typename C>
    class BinaryTree
    {
    public:
#error DO NOT FORGET THAT base HOLDS ITERATORS SO WE NEED TO RELINK base ON MOVE!!!
        //ctors
        //dtor
        //operator=
        //insert(unique, multiple)
        //erase(it, sent)
        //find(unique, multiple)
        //clear
        //empty
        //size
        //iterators
        //detach(it)
        //get allocator
        //get comparator
        //get memory requirements
    private:
        Detail::BinaryTreeNodeBase base; //end iterator + base.right == begin
        DeviceSize size;
        C comparator;
        Allocator allocator;
    };
};