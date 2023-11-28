#pragma once

namespace LEX
{
	ENUM(OperandType, uint8_t)
	{
		None,		//This is nothing in particular.
		Index,		//This is the argument index
		Register,
		Variable,	//This is a hard variable pointer.
		Type,
		Function,
		Member,
		Review,
		//Target,	//Target was when arguments and such where 2 seperate things.
		Literal,
		Total,


		Invalid = OperandType::Total,
	};
}