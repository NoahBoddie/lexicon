#pragma once

namespace LEX
{

	struct ModuleChain
	{
		ModuleChain() = default;

		ModuleChain(ParseModule* cur, ModuleChain* prev, ParsingStream* pro) : current{ cur }, previous{ prev }, process{ pro } {}


		ModuleChain(ParseModule* cur, ModuleChain*& prev) : current{ cur }, previous{ prev }, process{ prev->process }, relink{ &prev }
		{
			prev = this;
		}

		~ModuleChain()
		{
			if (relink)
				*relink = previous;
		}



		ModuleChain InheritChain(ParseModule* cur, ModuleChain*& prev)
		{
			ModuleChain chain{ cur, prev, process };
			chain.relink = &prev;
			return chain;
		}


		bool HasKeyword(std::string_view type)
		{
			if (!this)
				return false;

			ModuleChain* link = this;

			while (link)
			{
				if (link->current)
				{
					auto res = link->current->HasKeywordState(type);

					if (res == std::nullopt)
						return false;

					if (res.value() == true)
						return true;

				}

				link = link->previous;
			}

			return false;
		}


		ParseModule* current = nullptr;
		ModuleChain* previous = nullptr;

		ModuleChain** relink = nullptr;

		ParsingStream* process = nullptr;

	};


}