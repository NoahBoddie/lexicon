#pragma once

#include "Lexicon/ISpecial.h"
#include "Lexicon/Interfaces/IElement.h"

namespace LEX
{
	struct GenericBase;
	struct ITemplatePart;
	struct ITemplateBody;
	
	struct IGlobal;
	struct Global;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION_DERIVES(IGlobal, ISpecial)
			{

				virtual IGlobal* CheckGlobal(ITemplatePart* args) = 0;

				virtual Global* GetGlobal(ITemplateBody* args) = 0;

				//Attempts to revert value. If the global is const or a special part, it will fail to revert.
				// If reverted just with default it will create the default value, if not, it will attempt to use a routine to set
				// information. Will not throw.
				virtual bool Revert(bool just_default) = 0;


			};
		}

		CURRENT_VERSION(IGlobal, 1);
		
	}
	
	struct __declspec(novtable) IMPL_VERSION_DERIVES(IGlobalAbstract, IGlobal, IElement)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::IGlobal);


	};




#ifndef LEX_SOURCE
	//Only accessible outside of the source.
	struct IGlobal : public IGlobalAbstract {};
#endif
}


#ifdef LEX_SOURCE
#include "Lexicon/Engine/IGlobalImpl.h"
#endif