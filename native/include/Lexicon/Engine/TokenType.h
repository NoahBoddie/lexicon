#pragma once

namespace LEX
{
	//enum TokenType : __int8
	ENUM(TokenType, uint8_t)
	{
		Invalid,
		Number,			//Number literal
		//Integer,		//Integer literal//I wish to seperate into 2 categories integers and literals, so I know which to make.
		String,			//String literal
		Object,			//Object literal
		Boolean,		//Boolean literal
		Punctuation,
		Identifier,
		Keyword,
		Operator,
		Comment,		//Not actually used, just a used in searching. Later, there can perhaps be a tooltip system where it can preserve comments.
		Format,			//Used to designate the inline language statement(s) of the script. One type per script.
		Whitespace,		//Very rarely used, whitespace only exists when querying for preprocessors.
		Custom,			//Used when custom parsing is detected, thus detecting the data type is on the user
	};


	//Of the above, I can make some valid starting values, and then some valid continue values.
	//I'll be leaving this role to regex, the token stream will handle the assignment.


}