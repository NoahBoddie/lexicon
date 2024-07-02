#pragma once

namespace LEX
{

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

		String(std::string str)
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



		std::string ToString() const
		{
			return { _ptr, _size };
		}

		std::string_view ToView() const
		{
			return { _ptr, _size };
		}

		operator std::string() const
		{
			return ToString();
		}

		operator std::string_view() const
		{
			return ToView();
		}


		void SetData(const char* str, size_t length)
		{
			//If ownership becomes a worry, lexicon will be the thing that creates and manages these strings.

			if (length == -1)
				length = std::strlen(str);

			_ptr = new char[length];
			std::strncpy(_ptr, str, length);
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



	};
	REQUIRED_SIZE(String, 0x10);


}