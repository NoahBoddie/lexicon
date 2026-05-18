#include "Lexicon/RuntimeVariable.h"

//*Rmv
#include"Lexicon/Engine/IHierarchyTree.h"

namespace LEX
{

	void RuntimeVariable::AdjustOffset(TypeInfo* type) const
	{
		auto& offset = GetData().offset;

		if (!type) {
			offset = RunVarData::nil_offset;
			return;
		}

		auto& value = Ref();

		TypeInfo* var_type = value.GetTypeInfo();

		assert_if(!var_type) {
			//error
			return;
		}

		if (var_type->IsScriptObject() == false) {
			offset = RunVarData::nil_offset;
			return;
		}

		DONT_RELEASE("This requires a function in type info to handle this.");
		//TODO: this needs to have a virtual function handle this part.
		{
			auto var_tree = var_type->GetHierarchyTree();

			auto i = var_tree->GetInheritIndex(type->GetHierarchyTree());

			assert_if(i == -1) {
				//error
				return;
			}

			offset = static_cast<uint32_t>(i);
		}

	}

}