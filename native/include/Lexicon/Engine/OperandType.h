#pragma once

namespace LEX
{
	ENUM(OperandType, uint8_t)
	{
		None,		//This is nothing in particular.
		//I'm thinking that these 2 will be pure
		Differ,		//Number that uses negatives
		Index,		//Number without negatives
		IndexSplit,		//2 unsigned numbers
		DifferSplit,		//2 unsigned numbers
		Enum,
		//Want an ENUM type so I can just convert those automatically
		//end
		Parameter,
		LocalVar,
		Value,		//Like a variable, but exists to expressly be used for internal runtime processes
		Argument,
		Register,
		Global,		//This is a hard variable pointer.
		Variable,
		Type,
		Function,	//This is an IFunction pointer, and must be specialized before use
		Converter,	//A conversion function. Used to turn a runtime variable into another one.
		Callable,	//This is a callable unit, it comes specialized and may not actually be a function with a routine.
		Member,
		//Target,	//Target was when arguments and such where 2 seperate things.
		Literal,
		Template,	//Stores an ITemplatePart
		Routine,
		Specializable,
					
		Total,
		

		Any = OperandType::None,
		ArgumentIndex = 1 << 6,
		VariableIndex = 2 << 6,
		ParameterIndex = 3 << 6,
		DynamicIndex = OperandType::ArgumentIndex | OperandType::VariableIndex | OperandType::ParameterIndex,

		Invalid = OperandType::Total,
		Review = OperandType::Total,
	};
}