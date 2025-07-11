#pragma once

#include "Lexicon/VariableType.h"

//*src
#include "Lexicon/TypeInfo.h"
#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{
	struct TypeInfo;

	using c_string = const char*;

	struct String
	{

		~String()
		{
			if (_ptr)
				delete[] _ptr;
		}

		constexpr String() noexcept = default;

		String(const char* str)
		{
			SetData(str, -1);
		}

		String(std::string_view view)
		{
			SetData(view.data(), view.size());
		}

		String(const std::string& str)
		{
			SetData(str.data(), str.size());
		}

		String(std::string&& str)
		{
			SetData(str.data(), str.size());
		}


		String(const String& other)
		{
			SetData(other._ptr, other._size);
		}

		String(String&& other)
		{
			std::swap(_ptr, other._ptr);
			std::swap(_size, other._size);
		}


		String& operator=(const char* str)
		{
			SetData(str, -1);
			return *this;
		}

		String& operator=(std::string_view view)
		{
			SetData(view.data(), view.size());
			return *this;
		}

		String& operator=(std::string str)
		{
			SetData(str.data(), str.size());
			return *this;
		}


		String& operator=(const String& other)
		{
			SetData(other._ptr, other._size);
			return *this;
		}

		String& operator=(String&& other)
		{
			_ptr = std::exchange(other._ptr, nullptr);
			_size = std::exchange(other._size, 0);
			return *this;
		}



		std::string string() const
		{
			return { _ptr, _size };
		}

		std::string_view view() const
		{
			return { _ptr, _size };
		}


		operator std::string() const
		{
			return string();
		}

		operator std::string_view() const
		{
			return view();
		}




		void SetData(const char* str, size_t length)
		{
			//If ownership becomes a worry, lexicon will be the thing that creates and manages these strings.

			//This needs to add some, due to the null termination
			if (length == -1)
				length = std::strlen(str);

			if (!str || !length)
				return;

			_ptr = new char[length + 1];
			std::strncpy(_ptr, str, length);
			_ptr[length] = '\0';

			//_ptr = new char[length + 1];
			//strncpy_s(_ptr, length, str);
			//_ptr[length] = '\0';
			
			_size = length;
		}


		//I could get away with using a variant for this.
		char* _ptr = nullptr;
		size_t _size = 0;


		[[nodiscard]] inline const char* data() const noexcept { return _ptr; }
		[[nodiscard]] inline const char* c_str() const noexcept { return data(); }
		[[nodiscard]] inline size_t size() const noexcept { return _size; }


		std::strong_ordering operator<=>(const String& other) const
		{
			auto test = std::strncmp(_ptr, other._ptr, _size > other._size ? _size : other._size);

			if (test > 0)
				return std::strong_ordering::greater;
			if (test < 0)
				return std::strong_ordering::less;
			else
				return std::strong_ordering::equal;

		}

		bool operator==(const String& other) const = default;


		static TypeInfo* GetVariableType(const String*)
		{
			ITypeInfo* policy = IdentityManager::instance->GetTypeByOffset("STRING", 0);

			//Should already be specialized, so just sending it.
			return policy->FetchTypePolicy(nullptr);
		}


	};
	REQUIRED_SIZE(String, 0x10);



	template <>
	struct VariableType<std::string>
	{

		TypeInfo* operator()()
		{
			return String::GetVariableType(nullptr);
		}
	};

	template <>
	struct VariableType<std::string_view>
	{

		TypeInfo* operator()()
		{
			return String::GetVariableType(nullptr);
		}
	};

	inline void Testing()
	{
		String test;

		auto cast = static_cast<std::string>(test);
	}


}