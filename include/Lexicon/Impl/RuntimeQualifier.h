#pragma once

namespace LEX
{
	ENUM(RuntimeQualifier, uint8_t)
	{
		None = 0,
		Refr = 1 << 1,
	};
}