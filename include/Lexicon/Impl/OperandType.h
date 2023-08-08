#pragma once

namespace LEX::Impl
{
	//Should be OperandType
	enum struct OperandType : uint8_t
	{
		Index,
		Register,
		Variable,		//The variables innate or loaded into the routine
		Argument,		//The list of arguments currently loaded to be used.
		Target,
		Global,
		Literal,
		Constant,
		Invalid = 0xFF
	};
}