#pragma once


namespace LEX
{
	ENUM(BasicQualifier_, uint8_t)
	{
		None = 0,
		
		Volatility_		= 0b11 << 0,
		Modable			= 0b00 << 0,
		Mutable			= 0b01 << 0,
		Const			= 0b11 << 0,
		

		All = max_value<std::underlying_type_t<BasicQualifier_>>,
	};
}