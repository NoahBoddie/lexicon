#pragma once

#include "Lexicon/ObjectData.h"
#include "Lexicon/ObjectInfo.h"
#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Interfaces/InterfaceSingleton.h"

#include"Lexicon/Versioning.h"

//*src
#include "Lexicon/ObjectPolicy.h"
#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{

	//This should be hidden I think?
	struct OBJECT_POLICY;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION (ObjectPolicyManager)
			{

				virtual uint32_t GetIndexFromName(std::string_view name) = 0;

				virtual uint32_t GetIndexFromCategory(std::string_view category) = 0;

				virtual OBJECT_POLICY* GetObjectPolicy(uint32_t index) = 0;

 
				virtual OBJECT_POLICY* RegisterObjectType(std::vector<std::string_view> aliases, std::string_view category, TypeOffset range, DataBuilder builder, HMODULE source) = 0;

			};
		}

		CURRENT_VERSION(ObjectPolicyManager, 1);
	}
	//I would actually prefer all the above versions to be interface, and this here be the only valid version.
	struct IMPL_SINGLETON(ObjectPolicyManager)
	{

		uint32_t GetIndexFromName(std::string_view name) INTERFACE_METHOD;

		uint32_t GetIndexFromCategory(std::string_view category) INTERFACE_METHOD;

		OBJECT_POLICY* GetObjectPolicy(uint32_t index) INTERFACE_METHOD;


		OBJECT_POLICY* RegisterObjectType(std::vector<std::string_view> aliases, std::string_view category,
			TypeOffset range, DataBuilder builder, HMODULE source) INTERFACE_METHOD;



		OBJECT_POLICY* GetObjectPolicyFromName(std::string_view category)
		{
			auto index = GetIndexFromCategory(category);

			return GetObjectPolicy(index);
		}

		OBJECT_POLICY* GetObjectPolicyFromName(std::string& category)
		{
			auto result = GetObjectPolicyFromName(std::string_view{ category });
			return result;
		}


	};


	//Put this in implementation. Shit doesn't need to be actively used.
	inline void RegisterObjectType(std::string_view category, TypeOffset range, std::vector<std::string_view> aliases, ObjectVTable* vtable, DataBuilder builder)
	{
		HMODULE source = GetCurrentModule();


		auto* policy = ObjectPolicyManager::instance->RegisterObjectType(aliases, category, range, builder, source);

		policy->base = vtable;
		vtable->SetPolicy(policy);

		//load vtable into returned function for object policy.

	}


	//No category has to be used if it has declared an object Typename
	template <has_object_info T, typename... Ts>//T has to have an ObjectVTable
	void RegisterObjectType(std::string_view category, TypeOffset range = 0)
	{
		constexpr size_t type_count = sizeof...(Ts) + 1;

		const std::type_info& type = typeid(T);

		//Use GetObjectInfo for this.
		static ObjectVTable* vtable = GetObjectInfo<T>();

		DataBuilder builder = GenericDataBuilder<T>;

		std::array<std::string_view, type_count> alias_names{ GetTypeName<T>(), GetTypeName<Ts>()... };

		//load vtable into returned function for object policy.
		return RegisterObjectType(category, range, { std::begin(alias_names), std::end(alias_names) }, vtable, builder);
	}




	template <has_object_info T>//Only accepts types with ObjectInfo or whatever I'm calling it, implemented.
	uint32_t GetObjectPolicyID()
	{
		constexpr std::string_view name = GetTypeName<T>();
		constexpr uint32_t invalid = -1;
		static uint32_t index = invalid;


		if (index == invalid) {

			index = ObjectPolicyManager::instance->GetIndexFromName(name);
		}
		
		return index;
	}


	template <has_object_info T>//Only accepts types with ObjectInfo or whatever I'm calling it, implemented.
	OBJECT_POLICY* GetObjectPolicy()
	{
		uint32_t index = GetObjectPolicyID<T>();

		return ObjectPolicyManager::instance->GetObjectPolicy(index);
	}


}