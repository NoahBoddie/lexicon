#pragma once

namespace LEX
{
	/*
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

		//Ok, so reference needs to be able to represent multiple things. (Also note, may make a take for runtime variable to know which of these is which.
		//-NoRef- The default, enforces that something new must be created.
		//-GlobalLValue(ref&)- an LValue that is or belongs to a global variable that isn't another explicit ref type.
		//-LocalLValue(ref&&)- An LValue this is or belongs to a local variable and isn't some other explicit ref type, however it may accept global refs as well.
		//-ImplicitLValue(ref)- Contextually, turns either into a global or local LValue. If a return or out, global. If an in, parameter, or maybe local variable, it's local.
		// For local variables, the context of what's placed in it should likely help decide what it is.
		//-TempRef(ref!)- An temporary value produced either by the result of an evaluation or by conversion.
		//-AnyRef(ref?)- Can be all of the above. It's the object of references. Largely, it will make a reference out of just about anything it can, it it cannot, it will
		// submit it by value.

		//Special refs
		//-Out(out)- Beholden to the same rules as global
		


		
		//Groups-Helps extract entire groups like rutime qualifiers.
		Basic = Qualifier::Constness_,
		Runtime = Qualifier::Reference_,

		All = (unsigned char)-1,

	};
	//*/



	//May rename this to "Refness". Sounds dumb but fits and doesn't take up a common word
	ENUM(Reference, uint8_t)
	{
		Temp,			//The result of an expression, a value with no home
		Maybe,			//May be a reference to a variable or a temporary value
		Var,			//A local variable declared within this scope. Cannot be promoted, implicitly local
		Local,			//A reference to a possible local variable. Can be promoted to Scoped
		Scoped,			//A reference to a variable created outside of this
		Global,			//A reference to a global variable, existing externally, as a global variable, or detached and shared.
		Generic,		//Generic refs aren't any one type, but are context based. Should accept all except temp
	};

	ENUM (Constness, uint8_t)
	{
		
		Modable,
		Mutable,
		Const,
		Readonly,//Probably not going to be used
	};
	
	enum struct QualifierFlag : int8_t
	{
		None = 0,


		All = -1,
	};


	struct Qualifier
	{
	public:
		Qualifier() = default;

		Qualifier(Reference ref, Constness cnst, QualifierFlag flgs) :
			reference{ ref },
			constness{ cnst },
			flags{ flgs }
		{}

		constexpr bool IsConst() const
		{
			return constness == Constness::Const;
		}

		constexpr Constness GetConstNormalized() const
		{
			return constness == Constness::Mutable ? Constness::Modable : constness;
		}

		operator bool() const
		{
			return reinterpret_cast<const uint64_t&>(*this);
		}

		operator Reference& () { return reference; }
		operator Constness& () { return constness; }
		operator QualifierFlag& () { return flags; }

		Qualifier& operator=(const Reference& other) { reference = other; return *this; }
		Qualifier& operator=(const Constness& other) { constness = other; return *this; }
		Qualifier& operator=(const QualifierFlag& other) { flags = other; return *this; }
		
		auto operator <=>(const Qualifier&) const = default;

		QualifierFlag flags = QualifierFlag::None;

		//This will be 64 bytes to allow for fixed size

	private:

		//This fills the padding for future expansions to qualifiers. The budget having value means that it's unable to be read properly.
		uint8_t _budget[5]{};

	public:
		Reference reference = Reference::Temp;
		Constness constness = Constness::Modable;


	};
	REQUIRED_SIZE(Qualifier, 0x8);






	//TODO: PLEASE relocate filter equals.
	template <auto T>
	using TypeOf = decltype(T);

	template <auto Filter>
	requires(std::is_enum_v<TypeOf<Filter>>) constexpr bool FilterEquals(TypeOf<Filter> value, TypeOf<Filter> check)
	{
		using T = TypeOf<Filter>;

		using U = std::underlying_type_t<T>;

		U filtered = std::to_underlying(value) & std::to_underlying(Filter);

		return filtered == std::to_underlying(check);
	}

	template <auto Filter>
	requires(std::is_enum_v<TypeOf<Filter>>) constexpr TypeOf<Filter> FilterOR(TypeOf<Filter> value, TypeOf<Filter> add)
	{
		using T = TypeOf<Filter>;

		using U = std::underlying_type_t<T>;
		//We remove the filter from the value, and remove everything but the filter from add.
		U fil_ret = std::to_underlying(value) & ~std::to_underlying(Filter);
		U fil_add = std::to_underlying(add) & std::to_underlying(Filter);

		return static_cast<T>(fil_ret | fil_add);
	}
}