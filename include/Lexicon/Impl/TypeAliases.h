#pragma once

//Change name to aliasTypes

namespace LEX
{
	using Column = uint16_t;
	using Line = uint16_t;

	//Temporary
	using Object = int;
	using Variable = int;
	using GlobalVariable = int;

	namespace Impl
	{
		class Parser;

		typedef Record(ParseFunc)(Parser*, Record*);
	}
	
}