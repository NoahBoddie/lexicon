#pragma once

#include "OverloadFlag.h"



namespace LEX
{
	struct Overload;
	struct OverloadClause;
	struct ITypePolicy;

	struct OverloadKey
	{//The argument side of the overload.

		//True name: TryMatch
		virtual Overload Match(OverloadClause*, ITypePolicy* scope, Overload* prev, OverloadFlag& flag) = 0;


	};

}