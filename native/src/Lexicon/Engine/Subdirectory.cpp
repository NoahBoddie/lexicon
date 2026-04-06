#include "Lexicon/Engine/Subdirectory.h"


#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"

namespace LEX
{


	Script* Subdirectory::GetParentScript()
	{
		if (IsSubproject() == true) {
			return _parent->As<Script>();
		}
		return nullptr;
	}

	void Subdirectory::SetParent(Directory* parent)
	{
		if (_parent) {
			report::fault::error("Parent of {} has already been set ({})", _name, _parent->GetName());
			return;
		}

		if (auto script = parent->As<Script>())
		{
			_parent = script;
			GetFlags() |= Flag::kIsSubproject;
		}
		else if (auto repo = parent->As<Repository>()) {
			_parent = repo;
		}
		else {
			report::fault::error("Directory {} is not a project or script. ({})", parent->GetName(), magic_enum::enum_name(parent->GetComponentType()));
		}

		//cannot set parent here either
	}
}