#pragma once



namespace LEX
{
	using TypeIndex = uint32_t;
	using TypeOffset = uint32_t;


	struct TypeIdentity
	{
		uint32_t start = 0;
		TypeIndex index = 0;
		TypeOffset offset = 0;
	};


	struct Trival
	{
		inline static uint32_t trivalIndex = 8000;	//Index of the trival set of IDs
		inline static uint32_t trivalStart = 2000;	//The start and end of 
		inline static uint32_t trivalEnd = 2000;
	};


	struct TypeID
	{
	private:
		uint32_t _value = invalidValue;
	public:
		
		constexpr static uint32_t invalidValue = -1;
		constexpr static uint32_t invalid = -1;
		
		
		constexpr TypeID() = default;




		constexpr TypeID(uint32_t v) : _value { v }
		{
			//this validates whether the type actually exists or not, varifying the existence first.
		}

		static TypeID CreateID(uint32_t v)
		{
			//this forcibly assigns a value to the id
			TypeID id{};
			id._value = v;
			return id;
		}


		static TypeID CreateTrival(uint16_t code);
		
		TypeOffset GetOffset() const;

		TypeIndex GetIndex() const;

		TypeIdentity GetIdentity() const;



		constexpr operator uint32_t()
		{
			return _value;
		}





		bool IsValid() const
		{
			return _value == invalid;
		}

		bool IsTrival() const
		{
			return std::clamp<uint32_t>(_value, Trival::trivalStart, Trival::trivalEnd) == _value;
		}



		//auto operator<=>(const TypeID& other) const
		//{
		//	return _value <=> other._value;
		//}


		//auto operator<=>(const uint32_t other) const
		//{
		//	return _value <=> other;
		//}


	};
	static_assert(sizeof(TypeID) == 0x4);


	struct TypeCode
	{
		//Move to type id or its own section
		constexpr static TypeIndex badIndex = max_value<TypeIndex>;
		constexpr static TypeOffset badOffset = max_value<TypeOffset>;

		//Make make these const.
		TypeIndex index = badIndex;
		TypeOffset offset = badOffset;

		constexpr operator bool() const
		{
			return index == badIndex || offset == badOffset;
		}
		constexpr TypeCode() = default;


		//void _Set(TypeIndex i, TypeOffset o) const;

		static TypeCode Create(TypeIndex i, TypeOffset o)
		{
			TypeCode code{};
			code.index = i;
			code.offset = o;
			return code;
		}

		constexpr TypeCode(TypeIndex i, TypeOffset o) : index{ i }, offset{ o }
		{
			//TODO: TypeCodeWill need to verify just like TypeID, unless being created raw.
		}

	};



	/*
	//Beta 2 ID, outdated as well. Soon to be replaced by range based concept to reduce allocation waste.
	struct TypeIndex
	{
		uint16_t value;
	};

	struct TypeCode
	{
		uint16_t value;
	};

	struct TypeID
	{
		TypeIndex _index;
		TypeCode _code;
	};

	//*/
}
