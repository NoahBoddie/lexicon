#pragma once

#include "Lexicon/Number.h"
#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{
	struct common_type
	{
		//Common type is a class that keeps track of all the common types that may get used. Namely, boolean.

		//TODO: common_type needs to start returning the Type object instead of just the basic form


		static TypeInfo* boolean()
		{
			constexpr auto bool_settings = Number::Settings::CreateFromType<bool>();

			static TypeInfo* type = nullptr;

			if (!type){
				type = IdentityManager::instance->GetTypeByOffset("NUMBER", bool_settings.GetOffset())->FetchTypePolicy(nullptr);
			}

			return type;
		}


		static TypeInfo* uboolean()
		{
			constexpr auto bool_settings = Number::Settings{ NumeralType::Integral, Size::Bit, Signage::Unsigned, Limit::Bound };

			static TypeInfo* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetTypeByOffset("NUMBER", bool_settings.GetOffset())->FetchTypePolicy(nullptr);
			}

			return type;
		}

		static TypeInfo* integer64()
		{
			constexpr auto int_settings = Number::Settings::CreateFromType<int32_t>();

			static TypeInfo* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetTypeByOffset("NUMBER", int_settings.GetOffset())->FetchTypePolicy(nullptr);
			}

			return type;
		}



		static TypeInfo* void_t()
		{
			static TypeInfo* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetInherentType(InherentType::kVoid)->FetchTypePolicy(nullptr);
			}

			return type;
		}
		

		static TypeInfo* object()
		{
			static TypeInfo* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetTypeByOffset("CORE", 0)->FetchTypePolicy(nullptr);
			}

			return type;
		}

		static TypeInfo* voidable()
		{
			static TypeInfo* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetInherentType(InherentType::kVoidable)->FetchTypePolicy(nullptr);
			}

			return type;
		}


		static TypeInfo* type_info()
		{
			static TypeInfo* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetTypeByOffset("REFLECT_Type", 2)->FetchTypePolicy(nullptr);
			}

			return type;
		}
	


	private:
		
	};
}