#include "Lexicon/Engine/InheritNode.h"

#include "Lexicon/Engine/IHierarchyTree.h"

namespace LEX
{


    std::array<uint32_t, 2> InheritNode::hash(bool ignore_flags)
    {
        //The idea of this will be that the first number will just be

        std::array<uint32_t, 2> result{ startHash, startHash };

        
        if (ignore_flags || HasZeroWidthHash() == false) {
            if (!ignore_flags && IsVirtualInherited() == true) {
                result[1] = virtual_pos;
            }
            else {
                result[1] += (uint32_t)tree->GetHashRange();
            }
        }

        return result;
    }
    ITypeInfo* InheritNode::type() const
    {
        return unconst(tree)->GetHierarchyType();
    }

}