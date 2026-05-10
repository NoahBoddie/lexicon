#pragma once

namespace LEX
{
	enum struct RelateType
	{
		None = -1,
		Subdirectory, 
		Subproject,

		
		//Required,	//Cannot be compiled without it. Ideally should inherit member functions but need referencing for non-methods
		Imported,	//Can be mentioned without the script name, but will lose to names within the same script
		Included,	//Carries similar rules to things within the same script
		Nested,		//Used to get what's solely within itself. Will currently be unused.
	};
}