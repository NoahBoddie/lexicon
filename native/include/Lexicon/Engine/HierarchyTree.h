#pragma once

#include "Lexicon/Engine/InheritNode.h"
#include "Lexicon/Engine/IHierarchyTree.h"

namespace LEX
{

    struct HierarchyTree : public IHierarchyTree
    {
        //The point of the hierarchy tree is that it basically does not own members itself. Rather, 
        // it merely owns elements of inheritance.


        std::vector<InheritNode> inheritance;

    };
}