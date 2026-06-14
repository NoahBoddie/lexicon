#pragma once

#include "Lexicon/Number.h"
#include "Lexicon/Interfaces/IdentityManager.h"

#include "Lexicon/ITypeInfo.h"
namespace LEX
{
	struct common_type
	{
		//Common type is a class that keeps track of all the common types that may get used. Namely, boolean.

		//TODO: common_type needs to start returning the Type object instead of just the basic form


		static TypeInfo* boolean()
		{
			constexpr auto settings = Number::Settings::CreateFromType<bool>();

			static TypeInfo* type = nullptr;

			if (!type){
				type = NULL_OP(NULL_Q(IdentityManager::instance->GetTypeByOffset("NUMBER", settings.GetOffset()))->GetTypeInfo(nullptr));
			}

			return type;
		}

		static TypeInfo* string()
		{
			static TypeInfo* type = nullptr;

			if (!type) {
				type = NULL_OP(NULL_Q(IdentityManager::instance->GetTypeByOffset("STRING", 0))->GetTypeInfo(nullptr));
			}

			return type;
		}


		static TypeInfo* uboolean()
		{
			constexpr auto settings = Number::Settings{ NumeralType::Integral, Size::Bit, Signage::Unsigned, Limit::Bound };

			static TypeInfo* type = nullptr;

			if (!type) {
				type = NULL_OP(NULL_Q(IdentityManager::instance->GetTypeByOffset("NUMBER", settings.GetOffset()))->GetTypeInfo(nullptr));
			}

			return type;
		}

		static TypeInfo* integer64()
		{
			constexpr auto settings = Number::Settings::CreateFromType<int64_t>();

			static TypeInfo* type = nullptr;

			if (!type) {
				type = NULL_OP(NULL_Q(IdentityManager::instance->GetTypeByOffset("NUMBER", settings.GetOffset()))->GetTypeInfo(nullptr));
			}

			return type;
		}

		static TypeInfo* integer32()
		{
			constexpr auto settings = Number::Settings::CreateFromType<int32_t>();

			static TypeInfo* type = nullptr;

			if (!type) {
				type = NULL_OP(NULL_Q(IdentityManager::instance->GetTypeByOffset("NUMBER", settings.GetOffset()))->GetTypeInfo(nullptr));
			}

			return type;
		}

		static TypeInfo* floating()
		{
			constexpr auto settings = Number::Settings::CreateFromType<float>();

			static TypeInfo* type = nullptr;

			if (!type) {
				type = NULL_OP(NULL_Q(IdentityManager::instance->GetTypeByOffset("NUMBER", settings.GetOffset()))->GetTypeInfo(nullptr));
			}

			return type;
		}

		static TypeInfo* double_t()
		{
			constexpr auto settings = Number::Settings::CreateFromType<double>();

			static TypeInfo* type = nullptr;

			if (!type) {
				type = NULL_OP(NULL_Q(IdentityManager::instance->GetTypeByOffset("NUMBER", settings.GetOffset()))->GetTypeInfo(nullptr));
			}

			return type;
		}



		static TypeInfo* void_t()
		{
			static TypeInfo* type = nullptr;

			if (!type) {
				type = NULL_OP(NULL_Q(IdentityManager::instance->GetInherentType(InherentType::kVoid))->GetTypeInfo(nullptr));
			}

			return type;
		}
		

		static TypeInfo* null_t()
		{
			static TypeInfo* type = nullptr;

			if (!type) {
				type = NULL_OP(NULL_Q(IdentityManager::instance->GetInherentType(InherentType::kNull))->GetTypeInfo(nullptr));
			}

			return type;
		}



		static TypeInfo* object()
		{
			static TypeInfo* type = nullptr;

			if (!type) {
				type = NULL_OP(NULL_Q(IdentityManager::instance->GetTypeByOffset("CORE", 0))->GetTypeInfo(nullptr));
			}

			return type;
		}

		static TypeInfo* voidable()
		{
			static TypeInfo* type = nullptr;

			if (!type) {
				type = NULL_OP(NULL_Q(IdentityManager::instance->GetInherentType(InherentType::kVoidable))->GetTypeInfo(nullptr));
			}

			return type;
		}


		static TypeInfo* type_info()
		{
			static TypeInfo* type = nullptr;

			if (!type) {
				type = NULL_OP(NULL_Q(IdentityManager::instance->GetTypeByOffset("REFLECT", ComponentType::TypeInfo))->GetTypeInfo(nullptr));
			}

			return type;
		}
	

		template<detail::function_has_var_type T>
		static TypeInfo* get_from_type()
		{
			static TypeInfo* type = nullptr;

			if (!type) {
				type = GetVariableType<T>();
			}

			return type;
		}

	private:
		
	};
}