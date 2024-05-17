#pragma once


namespace LEX
{
	ENUM(DeclareSpecifier, uint8_t)
	{
		None				= 0,
		Static				= 1 << 0,
		Readonly			= 1 << 1,
		Const				= 1 << 2,
		Virtual				= 1 << 3,

		Access				= 0b111 << 4,
		Private				= 0b000 << 4,
		Protected			= 0b001 << 4,
		Public				= 0b011 << 4,
		Internal			= 0b100 << 4,
		InternalPrivate		= 0b100 << 4,
		InteralProtected	= 0b101 << 4,
		InternalPublic		= 0b111 << 4,
		

		All = max_value<std::underlying_type_t<DeclareSpecifier>>,
	};

	using Specifier = DeclareSpecifier;
}