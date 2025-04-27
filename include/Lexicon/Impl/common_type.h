#pragma once

#include "Lexicon/Number.h"
#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{
	struct common_type
	{
		//Common type is a class that keeps track of all the common types that may get used. Namely, boolean.

		//TODO: common_type needs to start returning the Type object instead of just the basic form


		static ITypeInfo* boolean()
		{
			constexpr auto bool_settings = Number::Settings::CreateFromType<bool>();

			static ITypeInfo* type = nullptr;

			if (!type){
				type = IdentityManager::instance->GetTypeByOffset("NUMBER", bool_settings.GetOffset());
			}

			return type;
		}


		static ITypeInfo* uboolean()
		{
			constexpr auto bool_settings = Number::Settings{ NumeralType::Integral, Size::Bit, Signage::Unsigned, Limit::Bound };

			static ITypeInfo* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetTypeByOffset("NUMBER", bool_settings.GetOffset());
			}

			return type;
		}

		static ITypeInfo* integer64()
		{
			constexpr auto int_settings = Number::Settings::CreateFromType<int32_t>();

			static ITypeInfo* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetTypeByOffset("NUMBER", int_settings.GetOffset());
			}

			return type;
		}



		static ITypeInfo* void_t()
		{
			static ITypeInfo* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetInherentType(InherentType::kVoid);
			}

			return type;
		}
		

		static ITypeInfo* object()
		{
			static ITypeInfo* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetTypeByOffset("CORE", 0)->GetTypePolicy(nullptr);
			}

			return type;
		}

		static ITypeInfo* voidable()
		{
			static ITypeInfo* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetInherentType(InherentType::kVoidable);
			}

			return type;
		}


		static ITypeInfo* type_info()
		{
			static ITypeInfo* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetTypeByOffset("REFLECT_Type", 2);
			}

			return type;
		}
	


	private:
		
	};
}