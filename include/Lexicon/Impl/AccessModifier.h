#pragma once


namespace LEX
{
	//Is access specifier.

	enum struct AccessModifier : uint8_t
	{//The base struct that handles this might be later named "ClassicInfos". Maybe. Will decide if it sounds dumb.

		Private,	//Only a given scope can observe it
		Protected,	//Only the scope and derived scopes can observe it (derived data types or required/imported/included scripts). Means different things between data type and 
		
		//ProtectedInternal,//extends internal to derived scripts. In a class functions much like internal.
		Public,

		Internal = 1 << 7,	//Only the script can observe it. Is a flag rather than another thing
		//Protected		Only the scope and derived scopes can observe it (derived data types or required/imported/included scripts). Means different things between data type and 
		//				script block usage.
	};
}
