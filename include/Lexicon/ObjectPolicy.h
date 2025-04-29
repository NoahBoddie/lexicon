#pragma once

#include "Lexicon/TypeID.h"
#include "Lexicon/ObjectPoolData.h"
#include "Lexicon/ObjectInfo.h"
#include "Lexicon/String.h"
//*src
#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{
	struct ObjectPolicy;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(ObjectPolicy)
			{

				virtual ObjectVTable* GetVTable() = 0;


				//*
				[[nodiscard]] virtual uint32_t ObtainPool() = 0;

				virtual void IncPoolRef(uint32_t i) = 0;


				virtual uint32_t DecPoolRef(uint32_t i) = 0;

				//DEPRECATED, use the one that obtains the pool within its use.
				virtual ObjectData InitializePool(uint32_t i, ObjectData data, bool is_ptr) = 0;

				virtual ObjectData InitializePool(ObjectData data, bool is_ptr) = 0;

				virtual ObjectData* RequestPool(uint32_t i) = 0;

				virtual void DestroyPool(uint32_t i) = 0;


				virtual uint32_t GetTypeIDFromOffset(TypeOffset) = 0;

				virtual ObjectData CreateData(uint32_t = 0) = 0;

				virtual uint32_t GetPolicyID() = 0;

				virtual TypeIndex GetCategoryIndex() = 0;
				virtual std::string_view GetCategoryName() = 0;
				//*/

				Object CreateObject(uint32_t);


			};
		}

		CURRENT_VERSION(ObjectPolicy, 1);
	}

	//TODO: Idea on object policies, they derive from object vtable, so they are effective stand-ins for questions around object infos.
	// Largely, they'll just point back to the object info.
	// Problem is, in order to do this I NEED to have it be multiple inheritance, this way newer functions added via more inheritance won't interfer with either
	// side.
	//But the problem is having a return that works between the source and the user. So here's what I do. A type alias, or perhaps a macro instead.
	// Should it be the source, Most uses of OBJECT_POLICY will be ObjectPolicyImpl. But for the user it's ObjectPolicy. This allows me to obscure this connection,
	// but also access the extra vtable without casting, AND the pointers are in the same location for it to continue treating it as an ObjectPolicy.

	//Problem is I'll need to put it on a pivot, mainly so I have 2 different vtables, but still have the object TypeBase able to be treated like an ObjectVTable.

	//So it might be something like IObjectPolicy, ObjectPolicy(Pivot), ObjectPolicyImpl OBJECT_POLICT(macro between ObjectPolicy and ObjectPolicyImpl).



	//This is
	struct __declspec(novtable) IMPL_VERSION_DERIVES(IObjectPolicy, ObjectPolicy), public IObjectVTable
	{
		//Make const plz
		Object CreateObject(uint32_t);
	};

#ifndef LEX_SOURCE
	struct __declspec(novtable) ObjectPolicy : public IObjectPolicy{};

#endif

	



}