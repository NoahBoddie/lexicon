#pragma once


namespace LEX
{
	union TypeID
	{
		//I think I'll make this trival actually.

		static constexpr uint8_t trivalBit = sizeof(std::byte) * 3 - 1;

	private:
		std::array<uint8_t, 4> _bytes;

		std::bitset<32> _bits;

		uint32_t _raw{ 0 };

	public:
		TypeID() = default;
		constexpr TypeID(uint8_t index, uint32_t code)
		{
			_raw = code;
			if (_bytes[0]) {
				//Invalid code was given or something.
				//All bytes on then?
				//throw nullptr;
			}

			_bytes[0] = index;
		}

		
		constexpr static TypeID CreateTrivalID(uint32_t code)
		{
			//Makes a Trival
			return{};
		}


		constexpr operator uint32_t()
		{
			return _raw;
		}

		constexpr bool IsTrival() const
		{
			return GetIndex() == 0 && _bits[trivalBit];
		}

		constexpr uint8_t GetIndex() const
		{
			return _bytes[0];
		}


		constexpr uint32_t GetCode() const
		{
			uint32_t result = 0x00FFFFFF & _raw;



			return result;
		}
		constexpr uint32_t GetTrivalCode() const
		{
			uint32_t remove = 0xFF000000 | (1 << trivalBit);
			//returns max bits because 0 is often going to be used.
			return IsTrival() ? _raw & remove : full_value<uint32_t>;
		}

		constexpr uint32_t GetID() const
		{
			//Should clear the presence
			return 0;
		}

		static TypeID Invalid()
		{
			return {};
		}

		auto operator<=>(const TypeID&) const = default;

		//TODO:Instead of redoing comparing ops for TypeID, I think it perhaps prudent to make a new comparison Less class for this
		//bool operator < (const TypeID a_rhs) const
		//{
		//	GetID() < a_rhs.GetID();
		//}

		//bool operator == (const TypeID a_rhs) const
		//{
		//	return GetID() == a_rhs.GetID();
		//}
	};
	static_assert(sizeof(TypeID) == 0x4);



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
