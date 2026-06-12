#pragma once

#include "Lexicon/ObjectData.h"
#include "Lexicon/ObjectInfo.h"
#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Interfaces/InterfaceSingleton.h"


//*src
#include "Lexicon/ObjectPolicy.h"
#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{
	struct ObjectHelper;

	//This should be hidden I think?
	struct ObjectPolicy;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION (ObjectPolicyManager)
			{

				virtual uint32_t GetIndexFromName(std::string_view name) = 0;

				virtual uint32_t GetIndexFromCategory(std::string_view category) = 0;

				virtual ObjectPolicy* GetObjectPolicy(uint32_t index) = 0;

 
				virtual uint32_t RegisterObject(std::unique_ptr<ObjectInfoBase>&& info, const std::string_view& name, const std::string_view& category,
					TypeOffset range, HMODULE source) = 0;

				virtual void RegisterAliases(uint32_t id, const std::span<std::string_view>& aliases) = 0;
			};

			
		}

		CURRENT_VERSION(ObjectPolicyManager, 1);
	}



	//Index in all of these mean ID. I'd like to reflect that please.


	//I would actually prefer all the above versions to be interface, and this here be the only valid version.
	struct IMPL_SINGLETON(ObjectPolicyManager)
	{

		uint32_t GetIndexFromName(std::string_view name) override;

		uint32_t GetIndexFromCategory(std::string_view category) override;

		ObjectPolicy* GetObjectPolicy(uint32_t index) override;


		uint32_t RegisterObject(std::unique_ptr<ObjectInfoBase>&& info, const std::string_view& name, const std::string_view& category,
			TypeOffset range, HMODULE source) override;

		void RegisterAliases(uint32_t id, const std::span<std::string_view>& aliases) override;


		ObjectPolicy* GetObjectPolicyFromName(const std::string_view& category)
		{
			auto index = GetIndexFromCategory(category);

			return GetObjectPolicy(index);
		}


		

		
	};


	template<has_object_info T>
	uint32_t RegisterObject(std::unique_ptr<ObjectInfoBase>&& info)
	{
		HMODULE source = GetCurrentModule();


		return ObjectPolicyManager::instance->RegisterObject(
			std::move(info), 
			GetTypeName<T>(),
			GetObjectCategory<T>(), 
			GetObjectRange<T>(),
			source);
	}


	template<has_object_info T, std::derived_from<ObjectInfoBase> Info>
	uint32_t RegisterObject()
	{
		return RegisterObject<T>(std::make_unique<Info>());
	}



	template<setting_is_object_info T>
	uint32_t RegisterObject()
	{
		return RegisterObject<T>(std::make_unique<ObjectSettings<T>>());
	}
	

	template <has_object_info T>//Only accepts types with ObjectInfo or whatever I'm calling it, implemented.
	uint32_t FetchObjectPolicyID()
	{
		constexpr std::string_view name = GetTypeName<std::remove_cvref_t<T>>();
		constexpr uint32_t invalid = -1;
		static uint32_t index = invalid;


		if (index == invalid) {

			index = ObjectPolicyManager::instance->GetIndexFromName(name);
		}

		return index;
	}

	template <has_object_info T>//Only accepts types with ObjectInfo or whatever I'm calling it, implemented.
	uint32_t GetObjectPolicyID()
	{
		constexpr std::string_view name = GetTypeName<std::remove_cvref_t<T>>();
		constexpr uint32_t invalid = -1;
		auto result = FetchObjectPolicyID<T>();

		if (result == invalid) {
			report::compile::critical("Object Policy '{}' not found.", name);
		}
		
		return result;
	}


	//Revisit handling this, right now I want to stop.
	
	template <typename... Aliases>
	void RegisterAliases(uint32_t id)
	{
		std::array<std::string_view, sizeof...(Aliases)> alias_names{ GetTypeName<Aliases>()... };
		return ObjectPolicyManager::instance->RegisterAliases(id, alias_names);
	}

	
	inline void RegisterAliases(const std::string_view& name, const std::span<std::string_view>& aliases)
	{
		uint32_t id = ObjectPolicyManager::instance->GetIndexFromName(name);

		return ObjectPolicyManager::instance->RegisterAliases(id, aliases);
	}

	template <has_object_info T>
	void RegisterAliases(const std::span<std::string_view>& aliases)
	{
		return ObjectPolicyManager::instance->RegisterAliases(GetObjectPolicyID<T>(), aliases);
	}

	template <typename... Aliases>
	void RegisterAliases(const std::string_view& name)
	{
		uint32_t id = ObjectPolicyManager::instance->GetIndexFromName(name);

		return RegisterAliases<Aliases...>(id);
	}

	template <has_object_info T, typename... Aliases>
	void RegisterAliases()
	{
		return RegisterAliases<Aliases...>(GetObjectPolicyID<T>());
	}










	template <has_object_info T>//Only accepts types with ObjectInfo or whatever I'm calling it, implemented.
	ObjectPolicy* FetchObjectPolicy()
	{
		uint32_t index = FetchObjectPolicyID<T>();

		return ObjectPolicyManager::instance->GetObjectPolicy(index);
	}


	template <has_object_info T>//Only accepts types with ObjectInfo or whatever I'm calling it, implemented.
	ObjectPolicy* GetObjectPolicy()
	{
		auto result = FetchObjectPolicy<T>();

		if (!result) {
			constexpr std::string_view name = GetTypeName<std::remove_cvref_t<T>>();
			report::compile::critical("Object Policy '{}' not found.", name);
		}

		return result;
	}


}