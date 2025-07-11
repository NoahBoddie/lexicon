#pragma once

//Change name to aliasTypes




namespace LEX
{
	using Column = uint16_t;
	using Line = uint32_t;
	
	//Temporary
	//using Object = int;
	//using Variable = int;
	
	//This is an example of what is to be put in registers, parameter arguments and such.
	// The idea is basically there are 3 types of variables.
	//Variables that are captured from one runtime to another,
	// or Variables that are from another function runtime

	//struct Syntax;
	//struct SyntaxBody;
	//using SyntaxRecord = BasicRecord<Syntax, SyntaxBody>;

	class ParsingStream;

	typedef Record(ParseFunc)(ParsingStream*, Record*);

	
}