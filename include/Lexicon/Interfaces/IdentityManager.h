#pragma once

//cut
#include "Lexicon/TypeID.h"

#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Interfaces/InterfaceSingleton.h"

#include "Lexicon/Versioning.h"


namespace LEX
{
	struct TypeID;
	struct ITypePolicy;
	struct PolicyBase;
	
	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IdentityManager)
			{
				virtual ITypePolicy* GetTypeByID(TypeID id) = 0;
				virtual uint32_t GetIDFromIndex(TypeIndex index) = 0;
				virtual TypeIndex GetIndexFromName(std::string_view name) = 0;
				virtual TypeIdentity GetIdentityFromID(TypeID id) = 0;
			};
		}

		CURRENT_VERSION(IdentityManager, 1);
	}



	struct IMPL_SINGLETON(IdentityManager)
	{
		ITypePolicy* GetTypeByID(TypeID id) INTERFACE_METHOD;//Isn't this supposed to be public?
		uint32_t GetIDFromIndex(TypeIndex index) INTERFACE_METHOD;
		TypeIndex GetIndexFromName(std::string_view name) INTERFACE_METHOD;
		TypeIdentity GetIdentityFromID(TypeID id) INTERFACE_METHOD;

		uint32_t GetIDFromName(std::string_view name) { return GetIDFromIndex(GetIndexFromName(name)); }
		ITypePolicy* GetTypeByOffset(std::string_view name, TypeOffset offset) { return GetTypeByID(GetIDFromName(name) + offset); }
		
	INTERNAL://These do not have any formal implementation outside of the source, and as such are privated and invalid outside of it.
		
		PolicyBase* GetBaseByID(TypeID id) INTERFACE_FUNCTION;

		uint32_t ObtainID(PolicyBase* policy) INTERFACE_FUNCTION;
		uint32_t GenerateID(std::string_view name, uint16_t range) INTERFACE_FUNCTION;
		TypeID ClaimID(PolicyBase* policy, uint32_t id) INTERFACE_FUNCTION;


		PolicyBase* GetBaseByOffset(std::string_view name, TypeOffset offset) { return GetBaseByID(GetIDFromName(name) + offset); }
		TypeID ClaimID(PolicyBase* policy, TypeIndex index, TypeOffset offset) { return ClaimID(policy, GetIDFromIndex(index) + offset); }
		TypeID ClaimID(PolicyBase* policy, std::string_view name, TypeOffset offset) { return ClaimID(policy, GetIndexFromName(name), offset); }
	};
}