#pragma once

//TODO: Mismatch name, DeclareSpecifier
namespace LEX
{


	ENUM(SpecifierFlag, int8_t)
	{
		None = 0,
		Static = 1 << 0,
		Readonly = 1 << 1,
		Const = 1 << 2,
		Virtual = 1 << 3,
		External = 1 << 4,

		All = -1,
	};

	ENUM(Access, uint8_t)
	{
		None,
		Private,
		Protected,
		Public,
		Internal = 1 << 3,
		InternalPrivate = Access::Private | Access::Internal,
		InternalProtected = Access::Protected | Access::Internal,
		InternalPublic = Access::Public | Access::Internal,
	};


	struct Specifier
	{
		SpecifierFlag flags = SpecifierFlag::None;

	private:
		uint8_t _budget[6]{};
	public:
		Access access = Access::None;
		

		operator bool() const
		{
			return reinterpret_cast<const uint64_t&>(*this);
		}

		operator Access& () { return access; }
		operator SpecifierFlag& () { return flags; }

		
		Specifier& operator=(const Access& other) { access = other; return *this; }
		Specifier& operator=(const SpecifierFlag& other) { flags = other; return *this; }


		bool IsStatic() const noexcept
		{
			return flags & SpecifierFlag::Static;
		}

		Specifier CopyWithFlags(SpecifierFlag flg) const
		{
			auto result = *this;

			result.flags |= flg;

			return result;
		}

	};
	REQUIRED_SIZE(Specifier, 0x8);

	/*
	//This will likely need expansion
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
		
		External			= 1 << 7,

		All = static_cast<std::underlying_type_t<DeclareSpecifier>>(-1),
	};

	using Specifier = DeclareSpecifier;
	//*/
}