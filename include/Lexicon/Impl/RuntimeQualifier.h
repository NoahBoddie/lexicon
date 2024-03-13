#pragma once

namespace LEX
{
	ENUM(RuntimeQualifier, uint8_t)
	{
		None = 0,
		Refr = 1 << 1,


		All =  max_value<std::underlying_type_t<RuntimeQualifier>>,
	};
}