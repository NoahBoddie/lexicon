#pragma once


namespace LEX
{
	enum struct AccessModifier
	{//The base struct that handles this might be later named "ClassicInfos". Maybe. Will decide if it sounds dumb.

		Private,	//Only a given scope can observe it
		Protected,	//Only the scope and derived scopes can observe it (derived data types or required/imported/included scripts). Means different things between data type and 
		Internal,	//Only the script can observe it.
		//ProtectedInternal,//extends internal to derived scripts. In a class functions much like internal.
		Public


		//Protected		Only the scope and derived scopes can observe it (derived data types or required/imported/included scripts). Means different things between data type and 
		//				script block usage.
	};
}
