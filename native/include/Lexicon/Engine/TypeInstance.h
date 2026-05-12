#pragma once

#include "Lexicon/TypeID.h"
#include "Lexicon/Engine/ITypeInfoImpl.h"
#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{

	template <std::derived_from<ITypeInfo> Type>
	struct TypeInstance : public Type
	{
		using Instance = TypeInstance<Type>;

		InstanceID GetInstanceID() const override
		{
			if (_instanceID == nil_instance_id) {
				IdentityManager::instance->GenerateInstanceID(unconst(this));
			}
			return _instanceID;

		}

		void SetInstanceID(InstanceID id, Badge<IdentityManager>) override
		{
			assert_if(_instanceID != nil_instance_id) {
				report::warn("instance id already assigned");
				return;
			}

			_instanceID = id;
		}

		mutable InstanceID _instanceID = nil_instance_id;


	};
}