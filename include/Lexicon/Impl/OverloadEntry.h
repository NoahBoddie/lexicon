#pragma once

#include "Conversion.h"
#include "QualifiedType.h"

namespace LEX
{
	//TODO:Delete me
	struct FakeType;

	struct OverloadCode
	{
		//Overload needs a place where it gets it's data for things like hashes, distance, and whether it's initialized or not.
		// this is what the finalize in entry would be doing.



		std::array<uint32_t, 2> hash{};

		uint32_t distance = 0;

		bool initialized = false;//If not initialized

		OverloadCode FinalizeOld(ITypePolicy* type, ITypePolicy* other)
		{



			//auto other_data = other->GetInheritData(type);

			if (other->IsInheritedFrom(type)  == false) {
				return type->CreateCode(nullptr);
			}
			//Even if it's virtually inherited, that's ok.

			return other->CreateCode(type);
		}



	};

	struct OverloadEntry
	{
		//I'd care about the padding and stuff here but its so small I do not care.

		FakeType* _type;



		QualifiedType type;//Only qualified because the going to place isn't important for 
		Conversion convert;
		ConvertResult convertType;
		OverloadCode code;

		size_t index;//The guide of where to put the given entries routine information.


		OverloadEntry FinalizeOld(FakeType* other);
		bool Finalize(OverloadEntry& other);
	};

}