#pragma once

namespace LEX
{
	ENUM(Qualifier, uint8_t)
	{
		None, 

		//Basic
		Constness_ = 0b11 << 0,//change the name of volitity
		Modable = 0b000 << 0,
		Mutable = 0b001 << 0,
		Const = 0b010 << 0,
		Readonly = 0b11 << 0,
		

		//Runtime
		Reference_ = 0b11 << 2,	//A variable. What this is is largely determined but where it is.
		RefL = 0b01 << 2,	//A persistent lvalue reference
		RefR = 0b10 << 2,	//A possibly volitile rvalue reference.

		//Groups-Helps extract entire groups like rutime qualifiers.
		Basic = Qualifier::Constness_,
		Runtime = Qualifier::Reference_,

		All = Qualifier::Basic | Qualifier::Runtime,

	};
	
}