#pragma once

#include "Lexicon/Interfaces/Info.h"

namespace LEX
{
	namespace Version
	{
		namespace _1
		{
			struct M_INTERFACE_VERSION(FunctionInfo)
			{

			};
		}

		CURRENT_VERSION(FunctionInfo, 1);
	}


	struct IMPL_VERSION(FunctionInfo, Info)
	{
		DEFINE_INFO_OFFSET(InfoType::FunctionInfo)
	public:



	};
}