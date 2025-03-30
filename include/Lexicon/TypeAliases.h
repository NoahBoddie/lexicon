#pragma once

//Change name to aliasTypes



#ifdef LEX_SOURCE
#define DECLARE_BASIC(mc_name) CONCAT(Basic,mc_name) = struct CONCAT(Abstract,mc_name)
#define BASIC_NAME(mc_name) STRINGIZE(CONCAT(Lexicon/Engine/Abstract,mc_name.h))
#else
#define DECLARE_BASIC(mc_name) CONCAT(Basic,mc_name) = struct CONCAT(I,mc_name)
#define BASIC_NAME(mc_name) STRINGIZE(CONCAT(CONCAT(Lexicon/I,mc_name),.h)
#endif

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

	using DECLARE_BASIC(Type);

	namespace Impl
	{
		class Parser;

		typedef Record(ParseFunc)(Parser*, Record*);
		//using ParseFunc = Record(Parser*, Record*);


	}
	
}