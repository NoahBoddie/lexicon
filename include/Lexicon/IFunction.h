#pragma once

#include "ISpecial.h"
#include "Lexicon/Interfaces/IElement.h"
#include "Lexicon/Impl/ComponentDetails.h"

namespace LEX
{
	struct Function;
	struct ITemplatePart;
	struct ITemplateBody;

	struct IFunction;
	
	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION_DERIVES(IFunction, ISpecial)
			{
				virtual bool IsConversion() const { return false; }
				virtual bool IsMethod() const { return false; }
				bool IsResolved() const override { return false; }

				//This should be hidden.
				virtual IFunction* CheckFunction(ITemplatePart* args) = 0;
				virtual Function* GetFunction(ITemplateBody* args) = 0;
			INTERNAL:
				//This shouldn't be internal should it? P
				virtual uint64_t GetProcedureData() const = 0;
			};
		}
		

		CURRENT_VERSION(IFunction, 1);
	}


	struct __declspec(novtable) IMPL_VERSION_DERIVES(IFunctionAbstract, IFunction, IElement)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::IFunction)

	};



#ifndef LEX_SOURCE
	//Only accessible outside of the source.
	struct IFunction : public IFunctionAbstract {};
#endif

}


#ifdef LEX_SOURCE
#include "Lexicon/Engine/IFunctionImpl.h"
#endif