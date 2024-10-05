#pragma once

namespace LEX
{
	//This is a list of build in types that exist before any other type comes into existence.
	// This includes types like void, object, trival, or trival tuple.
	
	//A different name could be ReservedTypes.
	ENUM(InherentType, uint8_t)
	{
		kVoid,
		kVoidable,//An interface all objects can be a part of, and the only thing void derives from.
		//kObject,//No Object, it may have a name as well as other elements. (Then I could just name it.
		//kTrival,
		//kTuple,
		_1,
			_4,
			_2,
			_3,
		kTotal,
	};
}