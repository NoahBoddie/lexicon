#include "Lexicon/Engine/InheritNode.h"

#include "Lexicon/Engine/IHierarchyTree.h"

namespace LEX
{


    std::array<uint32_t, 2> InheritNode::hash()
    {
        //The idea of this will be that the first number will just be

        if (IsVirtualInherited() == true) {
            return { 0, !HasNilHash() ? virtual_pos : 0 };
        }
        else {
            return { startHash, startHash + (uint32_t)tree->GetHashRange() };
        }
    }
    ITypeInfo* InheritNode::type() const
    {
        return unconst(tree)->GetHierarchyType();
    }

}