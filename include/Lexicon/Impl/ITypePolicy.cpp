#include "ITypePolicy.h"
#include "IdentityManager.h"

namespace LEX
{
		ITypePolicy::ITypePolicy()
		{
			IdentityManager::ObtainID(this);
		}

		ITypePolicy::ITypePolicy(uint32_t i)
		{
			IdentityManager::ClaimID(this, i);
		}

		ITypePolicy::ITypePolicy(std::string_view name, TypeOffset offset)
		{
			IdentityManager::ClaimID(this, name, offset);
		}
}