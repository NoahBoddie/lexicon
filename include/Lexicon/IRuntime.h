#pragma once

#include "Lexicon/TypeAliases.h"


namespace LEX
{

	struct Function;
	struct RuntimeVariable;
	class Runtime;
	struct ITemplateBody;


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IRuntime)
			{
				virtual IRuntime* GetParent() const = 0;


				virtual const Function* GetFunction() const = 0;

				virtual RuntimeVariable* GetDefault() const = 0;
				
				virtual void Log(const std::string_view& message, IssueLevel level = IssueLevel::Trace, std::source_location = std::source_location::current()) = 0;
				
				//These will honestly do nothing for a long time, but I'll make them at some point.
				virtual Column GetColumn() const = 0;
				virtual Line GetLine() const = 0;
				virtual std::string_view GetFile() const = 0;

				virtual ITemplateBody* AsBody() = 0;
			INTERNAL:
				virtual Runtime* AsRuntime() const = 0;
			};
		}

		CURRENT_VERSION(IRuntime, 1);
	}

	struct IMPL_VERSION(IRuntime) {};

}