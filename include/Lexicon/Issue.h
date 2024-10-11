//This goes in the precompiled header. These should be available far and wide.
#pragma once


#include "IssueCode.h"

namespace LEX
{
	
	namespace Issue
	{
		inline static IssueCode Nothing{ 1 };
	};


	INITIALIZE()
	{
		report::info(Issue::Nothing);
	}
}