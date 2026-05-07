#pragma once

#include "Lexicon/Interfaces/Info.h"

namespace LEX
{
	namespace Version
	{
		namespace _1
		{
			struct M_INTERFACE_VERSION(FunctionInfo_)
			{

			};
		}

		CURRENT_VERSION(FunctionInfo_, 1);
	}


	struct IMPL_VERSION(FunctionInfo_, Info)
	{
		DEFINE_INFO_OFFSET(InfoType::FunctionInfo)
	public:



	};
}