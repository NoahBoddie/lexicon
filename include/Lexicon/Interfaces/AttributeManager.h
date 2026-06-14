#pragma once

#include "Lexicon/AttributeData.h"

namespace LEX
{
	struct TypeBase;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(AttributeManager)
			{
				virtual bool RegisterNativeData(const std::string_view& name, AttributeBuilder builder) = 0;

			};
		}

		CURRENT_VERSION(AttributeManager, 1);
	}


	struct IMPL_SINGLETON(AttributeManager)
	{
		bool RegisterNativeData(const std::string_view& name, AttributeBuilder builder) override;

		template<std::derived_from<AttributeData> T>
		bool RegisterNativeData(const std::string_view& name)
		{
			constexpr auto builder = []() -> std::unique_ptr<AttributeData>
				{
					return std::make_unique<T>();
				};

			return RegisterNativeData(name, builder);
		}

		template<std::derived_from<AttributeData> T>
		bool RegisterNativeData() requires requires () { { T::ATTRIBUTE_NAME } ->std::convertible_to<std::string_view>; }
		{
			return RegisterNativeData<T>(T::ATTRIBUTE_NAME);
		}


	INTERNAL:
#ifdef LEX_SOURCE

		//Registers attributes that have yet to be registered properly.
		std::optional<bool>  RequestNativeData(const std::string_view& name, TypeBase* type);
		AttributeBuilder GetBuilder(const std::string_view& name);

#endif

	};
}