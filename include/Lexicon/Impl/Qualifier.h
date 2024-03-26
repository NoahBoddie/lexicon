#pragma once

namespace LEX
{
	ENUM(Qualifier, uint8_t)
	{
		None, 

		//Basic
		Volatility_ = 0b11 << 0,//change the name of volitity
		Modable = 0b00 << 0,
		Mutable = 0b01 << 0,
		Const = 0b11 << 0,
		

		//Runtime
		Refr = 1 << 2,


		//Groups-Helps extract entire groups like rutime qualifiers.
		Basic = Qualifier::Volatility_,
		Runtime = Qualifier::Refr,

		All = Qualifier::Basic | Qualifier::Runtime,

	};
	
}