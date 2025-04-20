#pragma once

//cut
#include "Lexicon/TypeID.h"

#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Interfaces/InterfaceSingleton.h"

#include "Lexicon/Versioning.h"

#include "Lexicon/InherentType.h"


namespace LEX
{
	struct TypeID;
	struct TypeBase;


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IdentityManager)
			{
				virtual BasicType* GetTypeByID(TypeID id) = 0;
				virtual uint32_t GetIDFromIndex(TypeIndex index) = 0;
				virtual TypeIndex GetIndexFromName(std::string_view name) = 0;
				virtual TypeIdentity GetIdentityFromID(TypeID id) = 0;

				virtual BasicType* GetInherentType(InherentType type) = 0;
			};
		}

		CURRENT_VERSION(IdentityManager, 1);
	}



	struct IMPL_SINGLETON(IdentityManager)
	{
		BasicType* GetTypeByID(TypeID id) INTERFACE_METHOD;//Isn't this supposed to be public?
		uint32_t GetIDFromIndex(TypeIndex index) INTERFACE_METHOD;
		TypeIndex GetIndexFromName(std::string_view name) INTERFACE_METHOD;
		TypeIdentity GetIdentityFromID(TypeID id) INTERFACE_METHOD;

		uint32_t GetIDFromName(std::string_view name) { return GetIDFromIndex(GetIndexFromName(name)); }
		BasicType* GetTypeByOffset(std::string_view name, TypeOffset offset) { return GetTypeByID(GetIDFromName(name) + offset); }
		BasicType* GetTypeByOffset(TypeIndex index, TypeOffset offset) { return GetTypeByID(GetIDFromIndex(index) + offset); }
		BasicType* GetInherentType(InherentType type) override;
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
		uint32_t GenerateID(std::string_view name, uint16_t range) INTERFACE_FUNCTION;
		TypeID ClaimID(TypeBase* policy, uint32_t id) INTERFACE_FUNCTION;


		TypeBase* GetBaseByOffset(std::string_view name, TypeOffset offset) { return GetBaseByID(GetIDFromName(name) + offset); }
		TypeID ClaimID(TypeBase* policy, TypeIndex index, TypeOffset offset) { return ClaimID(policy, GetIDFromIndex(index) + offset); }
		TypeID ClaimID(TypeBase* policy, std::string_view name, TypeOffset offset) { return ClaimID(policy, GetIndexFromName(name), offset); }
	};
}