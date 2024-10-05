#pragma once

#include "Lexicon/Number.h"
#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{
	struct common_type
	{
		//Common type is a class that keeps track of all the common types that may get used. Namely, boolean.



		static ITypePolicy* boolean()
		{
			constexpr auto bool_settings = Number::Settings::CreateFromType<bool>();

			static ITypePolicy* type = nullptr;

			if (!type){
				type = IdentityManager::instance->GetTypeByOffset("NUMBER", bool_settings.GetOffset());
			}

			return type;
		}


		static ITypePolicy* integer64()
		{
			constexpr auto int_settings = Number::Settings::CreateFromType<int32_t>();

			static ITypePolicy* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetTypeByOffset("NUMBER", int_settings.GetOffset());
			}

			return type;
		}



		static ITypePolicy* void_t()
		{
			static ITypePolicy* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetInherentType(InherentType::kVoid);
			}

			return type;
		}


		static ITypePolicy* voidable()
		{
			static ITypePolicy* type = nullptr;

			if (!type) {
				type = IdentityManager::instance->GetInherentType(InherentType::kVoidable);
			}

			return type;
		}



	private:
		
	};
}