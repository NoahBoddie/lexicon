#pragma once

//cut
#include "Lexicon/TypeID.h"

#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Interfaces/InterfaceSingleton.h"


#include "Lexicon/InherentType.h"


namespace LEX
{
	struct TypeID;
	struct TypeBase;

	struct ITypeInfo;

	
	struct TypeOrigin
	{
		std::string_view name;
		uint32_t startID = 0;
		uint32_t range = 0;
	};


	using TypeOffsetFn = TypeOffset(*)(const std::string_view&, const std::span<std::string_view>& args);

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IdentityManager)
			{
				//using ITypeInfo = struct LEX::ITypeInfo;

				//virtual bool GetOriginFromIndex(TypeOrigin & out, TypeIndex index) = 0;

				virtual ITypeInfo* GetTypeByID(TypeID id) = 0;
				
				virtual uint32_t GetIDFromIndex(TypeIndex index) = 0;
				virtual TypeIndex GetIndexFromName(std::string_view name) = 0;
				virtual TypeIdentity GetIdentityFromID(TypeID id) = 0;


				virtual ITypeInfo* GetInherentType(InherentType type) = 0;

				virtual TypeOffset GetTypeOffsetFromArgs(TypeIndex, const std::span<std::string_view>&args) = 0;

				virtual ITypeInfo* GetTypeFromInstanceID(InstanceID id) = 0;
			};
		}

		CURRENT_VERSION(IdentityManager, 1);
	}



	struct IMPL_SINGLETON(IdentityManager)
	{
		ITypeInfo* GetTypeByID(TypeID id) override;//Isn't this supposed to be public?
		uint32_t GetIDFromIndex(TypeIndex index) override;
		TypeIndex GetIndexFromName(std::string_view name) override;
		TypeIdentity GetIdentityFromID(TypeID id) override;
		TypeOffset GetTypeOffsetFromArgs(TypeIndex index, const std::span<std::string_view>& args) override;


		TypeOffset GetTypeOffsetFromArgs(const std::string_view& name, const std::span<std::string_view>&args)
		{
			return GetTypeOffsetFromArgs(GetIndexFromName(name), args);
		}

		uint32_t GetIDFromName(std::string_view name) { return GetIDFromIndex(GetIndexFromName(name)); }
		ITypeInfo* GetTypeByOffset(std::string_view name, TypeOffset offset) { return GetTypeByID(GetIDFromName(name) + offset); }
		ITypeInfo* GetTypeByOffset(TypeIndex index, TypeOffset offset) { return GetTypeByID(GetIDFromIndex(index) + offset); }
		ITypeInfo* GetInherentType(InherentType type) override;
		ITypeInfo* GetTypeFromInstanceID(InstanceID id) override;

		//ITypeInfo* 

	INTERNAL://These do not have any formal implementation outside of the source, and as such are privated and invalid outside of it.
		
		TypeBase* GetInherentBase(InherentType type) INTERFACE_FUNCTION;
		
		TypeBase* GetBaseByID(TypeID id) INTERFACE_FUNCTION;

		uint32_t ObtainID(TypeBase* policy) INTERFACE_FUNCTION;
		
		/// <summary>
		/// 
		/// </summary>
		/// <param name="name">The category name of the set of ids</param>
		/// <param name="range">The range of ids created between 0-range</param>
		/// <returns></returns>
		uint32_t GenerateID(std::string_view name, uint16_t range, TypeOffsetFn func = nullptr) INTERFACE_FUNCTION;
		TypeID ClaimID(TypeBase* policy, uint32_t id) INTERFACE_FUNCTION;


		TypeBase* GetBaseByOffsetArgs(const std::string_view& name, const std::span<std::string_view>& args)
		{
			TypeIndex index = GetIndexFromName(name);

			assert_if(index == -1)
				return nullptr;

			TypeOffset offset = GetTypeOffsetFromArgs(index, args);

			assert_if (index == -1)
				return nullptr;

			return GetBaseByOffset(index, offset);
		}


		TypeBase* GetBaseByOffset(TypeIndex index, TypeOffset offset) { return GetBaseByID(GetIDFromIndex(index) + offset); }
		TypeBase* GetBaseByOffset(std::string_view name, TypeOffset offset) { return GetBaseByID(GetIDFromName(name) + offset); }
		TypeID ClaimID(TypeBase* policy, TypeIndex index, TypeOffset offset) { return ClaimID(policy, GetIDFromIndex(index) + offset); }
		TypeID ClaimID(TypeBase* policy, std::string_view name, TypeOffset offset) INTERFACE_FUNCTION;

		void GenerateInstanceID(ITypeInfo* type) INTERFACE_FUNCTION;

	};
}