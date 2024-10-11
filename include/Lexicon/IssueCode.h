#pragma once

namespace LEX
{
	struct IssueCode
	{

		union
		{
			uint64_t value = 0;

			struct
			{
				uint32_t project;
				uint32_t code;
			};
		};
		//constexpr IssueCode(uint32_t p, uint32_t c) : project{p}, code {c}{}

		constexpr void SetProjectID(const char* value, size_t size)
		{
			for (int x = 0; x < size; x++)
			{
				auto y = size - x - 1;

				project |= (uint32_t)value[x] << (y * 8);
			}
		}

		constexpr void SetProjectID(std::string_view value)
		{
			return SetProjectID(value.data(), value.size());
		}


		constexpr IssueCode() = default;

		template <int I> requires(I >= 1 && I <= 5)
			constexpr IssueCode(const char(&value)[I], uint32_t c) : project{}, code{ c }
		{
			if constexpr (I != 1)
			{
				constexpr auto size = I - 1;

				for (int x = 0; x < size; x++)
				{
					auto y = size - x - 1;

					project |= (uint32_t)value[x] << (y * 8);
				}
			}
		}


		constexpr IssueCode(std::string_view value, uint32_t c) : project{}, code{ c }
		{
			if (value.size() > 4)
				//report::apply::warn("Incorrect project tag size {}", value.size());
				return;
			else
				SetProjectID(value.data(), value.size());
		}



		constexpr IssueCode(uint32_t c) : project{}, code{ c }
		{

		}


		constexpr operator uint64_t() const
		{
			return value;
		}
	};
}