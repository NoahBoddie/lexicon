#pragma once

#include "InheritData.h"

namespace LEX
{

	struct ITypePolicy;

	struct HierarchyData
	{
		//FakeType will become a part of something called Hierarchy data. TypeBases have this, but so do Specializations of generic types.

		//Id like it if in a test of ambiguity this always wins, but that might not be possible in this set up.
		ITypePolicy* _extends;//Deprecated* extends = nullptr;
		std::vector<InheritData> inheritance;

		std::vector<ITypePolicy*> members;


		uint32_t hashRange = 0;//Range is equal to zero to this number.

		uint32_t memberCount = 0;//Should bind classes increase this value any? Nah, probably handle in post.
		

		bool hasInternal = false;

	};


}