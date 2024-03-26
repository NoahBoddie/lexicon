#pragma once

#include "InfoBase.h"

namespace LEX
{
	class MemberInfo : public InfoBase
	{
	public:
		MemberInfo() = default;

		MemberInfo(size_t i) {}


		struct Settings : public InfoBase::Settings
		{
			bool isRuntimeMember = false;
		};

		virtual Qualifier GetQualifiers() const override
		{
			auto setting = GetData<Settings>();

			if (!setting.isRuntimeMember)
				setting.flags = setting.flags & ~Qualifier::Runtime;
			
			return GetData<Settings>().flags;
		}

		/*
		virtual BasicQualifier GetBasicFlags() const
		{
			return GetData<Settings>().basic;
		}

		virtual RuntimeQualifier GetRuntimeFlags() const
		{
			if (auto setting = GetData<Settings>(); setting.isRuntimeMember)
				return setting.runtime;

			return RuntimeQualifier::None;
		}
		//*/
	protected:
		AccessModifier _access = AccessModifier::Private;

	};

}
