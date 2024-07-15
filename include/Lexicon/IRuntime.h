#pragma once

#include "Lexicon/TypeAliases.h"


namespace LEX
{

	struct AbstractFunction;
	struct RuntimeVariable;
	class Runtime;
	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IRuntime)
			{
				virtual IRuntime* GetPreviousRuntime() const = 0;


				virtual const AbstractFunction* GetFunction() const = 0;

				virtual RuntimeVariable* GetDefault() const = 0;


				//These will honestly do nothing for a long time, but I'll make them at some point.
				virtual Column GetColumn() const = 0;
				virtual Line GetLine() const = 0;
				virtual std::string_view GetFile() const = 0;

				virtual IGenericArgument* AsBody() { return nullptr; }

			INTERNAL:
				virtual Runtime* AsRuntime() const = 0;
			};
		}

		CURRENT_VERSION(IRuntime, 1);
	}

	struct IRuntime : public Version::Current::IRuntime {};

}