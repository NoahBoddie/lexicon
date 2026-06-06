#pragma once

namespace LEX
{

	ENUM(LinkFlag, uint16_t)
	{
		None = 0,
		Loaded			= 1 << 0,   //Used to tell when all scripts have loaded
		Declaration		= 1 << 1,	//Used when all functions have finished being declared, meaning their names exist.
		Constant		= 1 << 2,	//Used to tell when constants have finished loading
		Attribute		= 1 << 3,	//Used to tell when attributes can be attached to a given object
		Definition		= 1 << 4,	//Used when all functions have finished being defined, meaning they can be called. Also links external funcs.
		// (for AVG, called in postpostload)

		External		= 1 << 5,	//Used for the reference of properties or formulas and as such must be handled after load.
		Object			= 1 << 6,	//Used for objects who's reference of may not be visable on the spot.

		
		Final			= 1 << 7,		//Used when all other link flags have gone out.
		Exit			= 1 << 8,		//Used when the program is exiting.


		Complete		= ((uint16_t)-1) & ~(LinkFlag::Final | LinkFlag::Exit),
		All				= LinkFlag::Loaded | LinkFlag::Object | LinkFlag::Declaration | LinkFlag::Definition | LinkFlag::External | LinkFlag::Constant | LinkFlag::Attribute,
		Any				= LinkFlag::All,
		//Final happens when all linking is done, notably, what also happens here is a bid for dependency.
		// Basically by now if it's not loaded properly, it will never be. Final also cannot be manually selected.
		//TODO: So for the above, Component::Link needs a wrapped function.

	};
}