#pragma once

namespace LEX
{

	ENUM(LinkFlag, uint8_t)
	{
		None,
		Loaded = 1 << 0,   //Used to tell when all scripts have loaded
		Declaration = 1 << 1,	//Used when all functions have finished being declared, meaning their names exist.
		Definition = 1 << 2,	//Used when all functions have finished being defined, meaning they can be called. Also links external funcs.
		// (for AVG, called in postpostload)
		Object = 1 << 3,	//Used for objects who's reference of may not be visable on the spot.

		External = 1 << 4,	//Used for the reference of properties or formulas and as such must be handled after load.


		Final = 1 << (sizeof(LinkFlag) * 8 - 1),


		All = LinkFlag::Loaded | LinkFlag::Object | LinkFlag::Declaration | LinkFlag::Definition | LinkFlag::External,
		Any = LinkFlag::All,
		//Final happens when all linking is done, notably, what also happens here is a bid for dependency.
		// Basically by now if it's not loaded properly, it will never be. Final also cannot be manually selected.
		//TODO: So for the above, Component::Link needs a wrapped function.

	};
}