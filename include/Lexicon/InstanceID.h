#pragma once

namespace LEX
{

	struct InstanceID
	{
		static constexpr uint32_t nil_value = (uint32_t)-1;

		constexpr InstanceID() noexcept = default;
		constexpr InstanceID(uint32_t v) noexcept : value{ v } {}



		uint32_t value = nil_value;


		constexpr auto operator<=>(const InstanceID& id) const noexcept = default;


		constexpr operator uint32_t() const noexcept
		{
			return value;
		}

		constexpr bool IsValid() const noexcept
		{
			return value != nil_value;
		}
	};
}