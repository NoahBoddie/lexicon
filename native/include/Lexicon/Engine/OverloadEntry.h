#pragma once

#include "Lexicon/Conversion.h"
#include "Lexicon/QualifiedType.h"



namespace LEX
{
	//TODO:Delete me
	struct FakeType;

	struct HierarchyData;

	struct OverloadCode
	{
		//Overload needs a place where it gets it's data for things like hashes, distance, and whether it's initialized or not.
		// this is what the finalize in entry would be doing.



		std::array<uint32_t, 2> hash{};

		uint32_t distance = 0;

		bool initialized = false;//If not initialized

		OverloadCode FinalizeOld(HierarchyData* type, HierarchyData* other);



	};

	struct OverloadEntry
	{
		//I'd care about the padding and stuff here but its so small I do not care.

		QualifiedType type;//Only qualified because the going to place isn't important for 
		Conversion convert;
		ConvertResult convertType;
		OverloadCode code;

		size_t index;//The guide of where to put the given entries routine information.
	};

}