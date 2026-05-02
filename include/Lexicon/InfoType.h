#pragma once

namespace LEX
{

	ENUM(InfoType, uint16_t)
	{
		Invalid = static_cast<uint16_t>(-1),

		Info = 0,
		VarInfo,
		
		kScriptedMax,

		LocalInfo,
		ParameterInfo,
		FieldInfo,
		GlobalBase,

	};
}