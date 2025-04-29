#pragma once

#include "ISpecial.h"

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
				virtual std::string_view GetName() const = 0;

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


#ifndef LEX_SOURCE
	//Only accessible outside of the source.
	struct IMPL_VERSION(IFunction) {};
#endif

}