#pragma once

#include "Lexicon/Engine/RoutineBase.h"
#include "Lexicon/Engine/Declaration.h"


namespace LEX
{


	struct GlobalData
	{
		std::string _name;

		//Variable stores this sure, but each special global will have a different variable, so I might as well use this
		// for interface
		Declaration _declared;

		//This is the default for a given global. Call to reset value. But, do note, this should be a formula, not a callable unit.
		//For this I may need something that can be specialized.


		//This is the initialization for the function. Should be tied to a function called reset that can be used on
		// globals.
		std::unique_ptr<RoutineBase> _init;

	};

}