#pragma once

#include "Lexicon/IType.h"

namespace LEX
{

	//Please move this
	ENUM(TypeRuleset, uint64_t)
	{
		None = 0,
		ReturnOpt = 1 << 0,	//Controls if when in a return the option of returning it is optional
		NoReturn = 1 << 1,	//Controls if the type can be used in a return or not
		NoVariable = 1 << 2,	//Specifies it cannot be used as a parameter or variable.
		NoGlobal = 1 << 3,	//Specifies it can't be used as a global (runtime classes have this)
	};

	struct TemplateType;

	struct AbstractType : public IType
	{
		//At a later point this will die and be forgotten. I seek to have a type that can handle most of hierarchies needs,
		// without the explicit need of having a hierarchy data explicitly existing. Might make it a reference to send a message.
		virtual HierarchyData* GetHierarchyData() const = 0;

		virtual TemplateType* AsTemplate() { return nullptr; }

		//TODO: Allow TypeRuleset to be a pure virtual. I need to chase down where they should be used.
		//virtual TypeRuleset GetRuleset() const = 0;
		virtual TypeRuleset GetRuleset() const
		{
			return TypeRuleset::None;
		}


		bool CheckRuleset(TypeRuleset rule) const
		{
			return GetRuleset() & rule;
		}

		bool TryRuleset(TypeRuleset rule, bool def = false) const
		{
			return !this ? def : CheckRuleset(rule);
		}
	};
}