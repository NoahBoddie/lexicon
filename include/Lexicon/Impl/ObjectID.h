#pragma once


namespace LEX
{
	class ObjectIndex
	{
		//Should have a type that can be submitted to get the object index. Will return 0 if invalid.
		//0 belongs to native types, mainly for literals to have some denoting flags. 
		// completely bare, it means the true type of it is completely invalid.

		uint8_t _value;
	public:


		//I realized variables own arrays, this does not.
		//constexpr bool IsArray()
		//{
		//	return i & 0x80;
		//}

		constexpr operator uint8_t ()
		{

			return _value;
		}
	};


	template <class T>
	uint8_t GetTypeIndex()
	{
		//The idea is that using this (with the project not via API) you could tell if an object is an object that can be transfered
		//Through API you can get what type it is.

		//Note, this only counts for the hardcoded types, this has no barring on struct types.
		return 0;
	}

	template <class T>
	constexpr uint8_t object_type_index = 0;

	union ObjectID
	{
		//static constexpr uint32_t bitsToRemove = 0b11111 << 26;
		//I change my mind about this, I'd like it to be able to account for arrays, because otherwise I can't have the types collected
		struct
		{

			uint8_t value[4];
			//ObjectIndex index;
		};
	private:
		uint32_t _raw;

	public:

		constexpr operator uint32_t()
		{
			//uint8_t result[4];
			//result[0] = index.GetValue();
			//for (int i = 1; i < 4; i++)
			//	result[i] = value[i - 1];

			return _raw;
		}

		constexpr bool IsArray() const
		{
			return false;
		}

		uint8_t GetIndex()
		{
			return value[0];
		}


		uint32_t GetSpecifier() const
		{
			uint32_t result = 0xFF000000 & _raw;



			return result;
		}

		constexpr uint32_t Value() const
		{
			//Should clear the presence 
			return 0;
		}

		static ObjectID Invalid()
		{
			return {};
		}

		//TODO:Instead of redoing comparing ops for ObjectID, I think it perhaps prudent to make a new comparison Less class for this
		bool operator < (const ObjectID a_rhs) const
		{
			return Value() < a_rhs.Value();
		}

		bool operator == (const ObjectID a_rhs) const
		{

			return Value() == a_rhs.Value();
		}
	};
	static_assert(sizeof(ObjectID) == 0x4);


}
