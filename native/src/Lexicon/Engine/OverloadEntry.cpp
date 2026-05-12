#include "Lexicon/Engine/OverloadEntry.h"

#include "Lexicon/Engine/TypeBase.h"
#include "Lexicon/Engine/InheritNode.h"
#include "Lexicon/Engine/IHierarchyTree.h"

namespace LEX
{
	OverloadCode OverloadCode::FinalizeOld(IHierarchyTree* tree, IHierarchyTree* other)
	{
		//auto other_data = other->GetInheritData(type);

		if (other->GetInheritNode(tree) == nullptr) {
			return tree->CreateCode(nullptr);
		}
		//Even if it's virtually inherited, that's ok.

		return other->CreateCode(tree);
	}
}