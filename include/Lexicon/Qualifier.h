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
	ENUM(Refness, uint8_t)
	{
		Temp,			//The result of an expression, a value with no home
		Maybe,			//May be a reference to a variable or a temporary value
		Auto,			//A local variable declared within this scope. Cannot be promoted, implicitly local
		Static,			//A global variable that isn't a reference
		Local,			//A reference to a possible local variable. Can be promoted to Scoped
		Scoped,			//A reference to a variable created outside of this
		Global,			//A reference to a global variable, existing externally, as a global variable, or detached and shared.
		Generic,		//Generic refs aren't any one type, but are context based. Should accept all except temp


			//Membered- Name may be subject to change but the idea would be that this is a type that is beholden to whatever type is currently
			// being viewed from the perspective of the QualifiedField that owns it. Generic might actually fit this bill.
			// So the idea 
	};

	ENUM (Constness, uint8_t)
	{
		
		Modable,
		Const,
		Readonly,	//Similar to const in that it won't allow the slot to be edited, but will allow it to be placed in non-const spaces
					// Think of it as int* const, it can still use non-const functions, the value of the slot can't be assigned however.
					// for struct types this should cause a copy to be used instead of the type itself
		ConstReadonly,
		Mutable,
		//Readonly probably should probably be before modable
	};
	
	
	ENUM (QualifierFlag, int8_t)
	{
		//TODO: As it turns out, I think all of these qualifier flags are actually closer to being solution flags. None of these are
		// actually to be used o a qualified type.

		//Please note QualifierFlags aren't to be observed in an official capacity, they're mostly used for compiling. They also may be subject to change.
		None = 0,
		Params = 1 << 0,
		ImplicitReadonly = 1 << 1,
//#ifdef LEX_SOURCE
		
		//Initialized,		//Used when a value can be initialized. Think for out and in
		//Open,				//The idea would be the reference can now be reassigned
		//ExactRef,			//A flag that would exist to say "this type cannot be promoted". Assigned if a reference has roughly the same ref as it's expression
							// this can be cleared if it has been reassigned previously (to which it is then a question).
		//ReadOnly,			//Would be used for in
		//WriteOnly,		//Would be used for out
		Promoted = 1 << 4,	//Flag to denote promoted refs. Promoted refs cannot be used in any space other than loading, but can be converted into a bool

		All = -1,
//#endif
	};


	struct Qualifier
	{
	public:
		Qualifier() = default;

		Qualifier(Refness ref, Constness cnst, QualifierFlag flgs) :
			reference{ ref },
			constness{ cnst },
			flags{ flgs }
		{}

		constexpr Qualifier(Refness ref) noexcept :
			reference{ ref },
			constness{ Constness::Mutable },
			flags{ QualifierFlag::None }
		{}


		constexpr Qualifier(Constness cnst) noexcept :
			reference{ Refness::Temp },
			constness{ cnst },
			flags{ QualifierFlag::None }
		{}



		constexpr bool IsConst() const noexcept
		{
			switch (constness)
			{
			case Constness::Const:
			case Constness::ConstReadonly:
				return true;

			default:
				return false;
			}
		}

		constexpr bool IsReadonly() const noexcept
		{
			switch (constness)
			{
			case Constness::Readonly:
			case Constness::ConstReadonly:
				return true;

			default:
				return flags & QualifierFlag::ImplicitReadonly;
			}
		}

		


		constexpr Constness GetConstness() const
		{
			return constness == Constness::Mutable ? Constness::Modable : constness;
		}

		constexpr std::optional<bool> IsReference(std::optional<bool> if_null = std::nullopt) const noexcept
		{
			std::optional<bool> result;
			switch (reference)
			{
			case Refness::Maybe:
				result = std::nullopt;
				break;

			case Refness::Global:
			case Refness::Local:
			case Refness::Scoped:
			case Refness::Generic:
				result = true;
				break;

			default:
				result = false;
				break;
			}

			if (result == std::nullopt)
				result = if_null;

			return result;
		}



		constexpr bool IsReference(bool if_null) const noexcept
		{
			return IsReference(std::optional<bool>{if_null}).value();
		}

		//Is Solution capable of being referenced in its current capacity
		constexpr bool IsReferential() const noexcept
		{
			switch (reference)
			{
			case Refness::Global:
			case Refness::Local:
			case Refness::Scoped:
			case Refness::Auto:
			case Refness::Static:
				return true;

			default:
				return false;
			}
		}

		constexpr bool IsConstnessEqual(const Qualifier& other) const noexcept
		{
			if (constness != other.constness) {
				return false;
			}

			if (flags & QualifierFlag::ImplicitReadonly != other.flags & QualifierFlag::ImplicitReadonly) {
				return false;
			}

			return true;
		}

		bool PromoteRefness()
		{
			switch (reference)
			{
				case Refness::Maybe:
					reference = Refness::Local;
					break;

				case Refness::Local:
					reference = Refness::Scoped;
					break;

				case Refness::Scoped:
					reference = Refness::Global;
					break;

				case Refness::Global:
					return false;

				case Refness::Auto:
				case Refness::Static:
				case Refness::Generic:
				case Refness::Temp:

			default:
				return false;
			}

			flags |= QualifierFlag::Promoted;
			return true;
		}

		void ClearPromotion()
		{

		}

		constexpr bool IsPromoted() const
		{
			return flags & QualifierFlag::Promoted;
		}


		void MakeReadonly(bool implicit)
		{
			if (implicit) {
				flags |= QualifierFlag::ImplicitReadonly;
				return;
			}

			switch (constness)
			{
			default:
				constness = Constness::Readonly;
				break;

			case Constness::Const:
			case Constness::ConstReadonly:
				constness = Constness::ConstReadonly;
				break;

			}
		}

		void MakeConst()
		{
			switch (constness)
			{
			default:
				constness = Constness::Const;
				break;

			case Constness::Readonly:
			case Constness::ConstReadonly:
				constness = Constness::ConstReadonly;
				break;

			}
		}

		operator bool() const
		{
			return reinterpret_cast<const uint64_t&>(*this);
		}

		//operator Refness& () { return reference; }
		//operator Constness& () { return constness; }
		//operator QualifierFlag& () { return flags; }

		//Qualifier& operator=(const Refness& other) { reference = other; return *this; }
		//Qualifier& operator=(const Constness& other) { constness = other; return *this; }
		//Qualifier& operator=(const QualifierFlag& other) { flags = other; return *this; }
		
		auto operator <=>(const Qualifier&) const = default;

		auto& QualifierFlags()
		{
			return flags;
		}

		QualifierFlag flags = QualifierFlag::None;

		//This will be 64 bytes to allow for fixed size

	private:

		//This fills the padding for future expansions to qualifiers. The budget having value means that it's unable to be read properly.
		uint8_t _budget[5]{};

	public:
		Refness reference = Refness::Temp;
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