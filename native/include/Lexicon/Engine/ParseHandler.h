#pragma once

#include "Lexicon/Engine/ParseModule.h"

namespace LEX::Impl
{

	using ModuleBuilder = std::unique_ptr<ParseModule>(*)();



	struct ParseHandler
	{
	public:
		static void AddBuilder(ModuleBuilder build);



		static std::vector<std::unique_ptr<ParseModule>> BuildModules();


	private:
		static void _CheckSort();


		//Not sure if I really want one list for this, maybe when requesting the depth I'll pull out only ones of a certain depth
		inline static std::vector<ModuleBuilder> _buildList;
		inline static bool _sorted = true;

	};

}
