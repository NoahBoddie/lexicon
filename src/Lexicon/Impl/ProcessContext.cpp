#include "Lexicon/Impl/ProcessContext.h"

namespace LEX::Impl
{
	bool ProcessContext::HasKeyword(std::string_view type)
	{
		return false;
	}

	bool ProcessContext::ContextAllowed(ProcessContext* context, ProcessChain* link) 
	{ 
		return true; 
	}


	bool ProcessContext::IsContextType(std::string_view type)
	{
		return GetContext().find(type) != std::string_view::npos;
	}



	//probably wont even use this.
	bool ProcessChain::IsContextType(std::string_view type)
	{
		//I guess? For safeness?
		if (!this)
			return false;

		ProcessChain* link = this;

		while (link)
		{
			if (link->current->IsContextType(type) == true)
				return true;

			link = link->previous;
		}

		return false;
	}

	bool ProcessChain::HasContext(ProcessContext* context)
	{
		if (!context || !this)
			return false;


		ProcessChain* link = this;

		while (link)
		{
			if (link->current == context)
				return true;

			link = link->previous;
		}

		return false;
	}

	ProcessChain ProcessChain::InheritChain(ProcessContext* cur, ProcessChain* prev)
	{
		return ProcessChain{ cur, prev, process };
	}



	ProcessChain IProcess::CreateChain()
	{
		return ProcessChain{ nullptr, nullptr, this };
	}
}