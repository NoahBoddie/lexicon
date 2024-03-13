#pragma once


namespace LEX
{
	ENUM(DeclareSpecifier, uint8_t)
	{
		None = 0,
		Static = 1 << 0,
		Readonly = 1 << 1,
		Const = 1 << 2,
		//Private, Public, Internal, Protected

		All = max_value<std::underlying_type_t<DeclareSpecifier>>,
	};
}