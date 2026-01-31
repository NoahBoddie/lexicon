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
		Variable,
		Value,		//Like a variable, but exists to expressly be used for internal runtime processes
		Argument,
		Register,
		Global,		//This is a hard variable pointer.
		Type,
		Function,	//This is an IFunction pointer, and must be specialized before use
		Convert,	//A conversion function. Used to turn a runtime variable into another one.
		Callable,	//This is a callable unit, it comes specialized and may not actually be a function with a routine.
		Member,
		Review,
		//Target,	//Target was when arguments and such where 2 seperate things.
		Literal,

		Total,




		
		ArgumentIndex = 1 << 6,
		VariableIndex = 2 << 6,
		ParameterIndex = 3 << 6,
		DynamicIndex = OperandType::ArgumentIndex | OperandType::VariableIndex | OperandType::ParameterIndex,

		UsesValue = 1 << 7,	//If a given type uses a value, it means that in an attempt to get an index or differ, it pulls a variable
		IndexValue = OperandType::Index | UsesValue,
		DifferValue = OperandType::Differ | UsesValue,
		Invalid = OperandType::Total,
	};
}