#pragma once

#include "Lexicon/Impl/ParseModule.h"

namespace LEX::Impl
{
	//I might template this
	struct ParseHandler
	{
	public:
		static void AddModule(ParseModule* mdl);



		static GENERIC_ARRAY(ParseModule*) GetModuleList();

		//Not sure if I actually want this
		static ParseModule* GetModuleByName(std::string_view);

	private:
		static void _CheckSort();


		//Not sure if I really want one list for this, maybe when requesting the depth I'll pull out only ones of a certain depth
		inline static GENERIC_ARRAY(ParseModule*) _moduleList;
		inline static bool _sorted = true;

	};


}
