#pragma once

#include "OverloadFlag.h"



namespace LEX
{
	struct Overload;
	struct OverloadClause;
	struct ITypeInfo;
	struct QualifiedType;
	
	struct OverloadKey
	{//The argument side of the overload.

		virtual QualifiedType GetTarget() const;

		//True name: TryMatch
		virtual Overload Match(OverloadClause*, ITypeInfo* scope, Overload* prev, OverloadFlag& flag) = 0;


	};

}