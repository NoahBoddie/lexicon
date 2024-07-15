#pragma once

namespace LEX
{
	ENUM(OverloadFlag, uint8_t)
	{
		None,
		ReqFilled = 1 << 0,		//Once required gets filled, this gets ticked so that the overload doesn't use the improper overload.
		UsesDefault = 1 << 1,	//If it uses 
		UsesOptional = 1 << 2,	//Manually defining an optional will result in no optionals being able to be implicitly used.
		NoConvert = 1 << 3,		//If no conversions are used in one, converting ones are no longer valid.


		TargetOpt = 1 << 4,		//If using a target is optional this will be ticked.
		AllAccess = 1 << 5,		//If all access is used, there are no rules about conversions.
		
		//Since these 2 are kinda the same I'm thinking of implementing 
		Failure = 1 << 6,
		Ambiguous = 1 << 7,
	};

}