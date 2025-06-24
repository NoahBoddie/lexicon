#pragma once

#include "InfoBase.h"

namespace LEX
{
	struct MemberInfo :public InfoBase
	{
		//This pivot is still going to need to exist for all the reasons that you'd imagine something like this would need. They sorta need to play proxy to the
		// specializations of both functions and for variables, so first this will need overloads, and each of those will need their respective specialization.


		bool IsStatic() const
		{
			return specifiers.IsStatic();
		}
	};

}
