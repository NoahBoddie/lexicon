#include "TypePolicy.h"
#include "IdentityManager.h"

namespace LEX
{

	PolicyBase::PolicyBase()
	{
		IdentityManager::ObtainID(this);
	}

	PolicyBase::PolicyBase(uint32_t i)
	{
		//SetTypeID(0); return;

		IdentityManager::ClaimID(this, i);
	}

	PolicyBase::PolicyBase(std::string_view name, TypeOffset offset)
	{
		//SetTypeID(0); return;
		IdentityManager::ClaimID(this, name, offset);
	}


}