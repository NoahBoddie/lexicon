#pragma once

namespace LEX
{
	//This is a list of build in types that exist before any other type comes into existence.
	// This includes types like void, object, trival, or trival tuple.

	//A different name could be ReservedTypes.
	ENUM(InheritType, uint8_t)
	{
		kVoid,
		kObject,
		kTrival,
		Tuple,
		kTotal,
	};
}