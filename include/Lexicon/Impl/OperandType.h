#pragma once

namespace LEX
{
	ENUM(OperandType, uint8_t)
	{
		None,		//This is nothing in particular.
		Differ,		//Number that uses negatives
		Index,		//Number without negatives
		Register,
		Variable,	//This is a hard variable pointer.
		Type,
		Function,	//This is an IFunction pointer, and must be specialized before use
		Callable,	//This is a callable unit, it comes specialized and may not actually be a function with a routine.
		Member,
		Review,
		//Target,	//Target was when arguments and such where 2 seperate things.
		Literal,
		Total,


		Invalid = OperandType::Total,
	};
}