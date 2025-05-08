#pragma once

namespace LEX
{

	//These are now used for the argument to tell the parameter it's situation.
	ENUM(OverloadFlag)
	{
		None,
		DefFilled = 1 << 0,		//Once required gets filled, this gets ticked so that the overload doesn't use the improper overload.
		StatesTemplate = 1 << 1,	//Marked once if stated is used. Crossing into default while this is active is a no no.
		StatesArgument = 1 << 2,	//Marked once if stated is used. Crossing into default while this is active is a no no.
		NoConvert = 1 << 3,		//If no conversions are used in one, converting ones are no longer valid.


		TargetOpt = 1 << 4,		//If using a target is optional this will be ticked.
		AllAccess = 1 << 5,		//If all access is used, there are no rules about conversions.

		IsTemplate = 1 << 6,

		Clearable = OverloadFlag::IsTemplate | OverloadFlag::DefFilled,//These flags are cleared between call and temp args given.
	};


}