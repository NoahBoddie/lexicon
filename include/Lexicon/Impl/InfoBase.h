#pragma once


#include "Field.h"

namespace LEX
{

	struct InfoBase : public Field
	{
	public:
		struct Settings
		{
			BasicQualifier basic;
			RuntimeQualifier runtime;
		};



		InfoBase() = default;

		std::string name{};

		//Instead, you can just store basic qualfiers here. Percieve them any way you'd like.
		// it's upto the derived.
		size_t _data{};



		//Mainly used for members and function variables. But since I've yet to see anything that remotely 
		// outside these 2 uses, I think this suits just fine.
		size_t index = max_value<size_t>;

	protected:
		


		template <typename T> requires (sizeof(T) <= 0x8)
			T& DataAs()
		{
			return reinterpret_cast<T&>(_data);
		}

		template <typename T> requires (sizeof(T) <= 0x8)
			const T& DataAs() const
		{
			return reinterpret_cast<const T&>(_data);
		}

		template <typename T> requires (sizeof(T) <= 0x8)
		T GetData() const
		{
			return DataAs<T>();
		}

	public:
		//move lower
		virtual BasicQualifier GetBasicFlags() const = 0;

		virtual RuntimeQualifier GetRuntimeFlags() const = 0;


		size_t GetFieldIndex() const override
		{
			return max_value<size_t>;
		}


		virtual operator bool() const
		{
			//Should this be and?
			return name != "" || _data;
		}
	};

}
