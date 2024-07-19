#pragma once

#include "Lexicon/IGlobal.h"
#include "Lexicon/Interfaces/Element.h"


namespace LEX
{
	struct ICallableUnit;

	struct GlobalData
	{
		std::string _name;
		
		//This is the default for a given global. Call to reset value. But, do note, this should be a formula, not a callable unit.
		ICallableUnit* _init = nullptr;
		
	};



	class GlobalBase : public virtual IGlobal, public SecondaryElement, public GlobalData
	{
	public:
		IGlobal* AsGlobal() override { return this; }


		//I may need a new type of linker for this, so the very most default value can be set for globals, then I can use them
		// in functions.
		//Actually, it would probably be something like external would be post definition. But on definition, simple values are filled.
		// to prevent error when trying to use them.

		void LoadFromRecord(Record& target) override;

		void OnAttach() override;

		virtual LinkResult OnLink(LinkFlag flags) override;

		virtual LinkFlag GetLinkFlags() override;

	};

}