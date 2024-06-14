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