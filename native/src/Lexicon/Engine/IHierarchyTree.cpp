#include "Lexicon/Engine/IHierarchyTree.h"

#include "Lexicon/Engine/MemberData.h"
#include "Lexicon/Engine/InheritNode.h"



namespace LEX
{
    size_t IHierarchyTree::GetFieldCount()
    {
        if (auto members = GetMembers()) {
            return members->fields.size();
        }

        return 0;
    }

    bool IHierarchyTree::DerivesFrom(const IHierarchyTree* other)
    {
        return GetInheritNode(other);
    }
}